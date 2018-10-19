#pragma once
/*

_____.___.          __                              __   .__
\__  |   |  ____  _/  |_  _____     ____    ____  _/  |_ |  |__    ____  _______
 /   |   |_/ __ \ \   __\ \__  \   /    \  /  _ \ \   __\|  |  \ _/ __ \ \_  __ \
 \____   |\  ___/  |  |    / __ \_|   |  \(  <_> ) |  |  |   Y  \\  ___/  |  | \/
 / ______| \___  > |__|   (____  /|___|  / \____/  |__|  |___|  / \___  > |__|
 \/            \/              \/      \/                     \/      \/
__________                   _____ .__ .__
\______   \_______   ____  _/ ____\|__||  |    ____  _______
 |     ___/\_  __ \ /  _ \ \   __\ |  ||  |  _/ __ \ \_  __ \
 |    |     |  | \/(  <_> ) |  |   |  ||  |__\  ___/  |  | \/
 |____|     |__|    \____/  |__|   |__||____/ \___  > |__|

 
 Yet Another Profiler (YAP) v 0.1 2015/05/12

 * 0 : Compilation prerequisites
	to enable :
	 #define YAP_ENABLE
	 define YAP_IMPL in one cpp before .h inclusion
	 define YAP_IMGUI if you want Ocornut's ImGui

 * 1 :  Glossary
	 - A phase is a process in your app : Game init, Game loading res, menus, gameplay,...
	 - A section is a part of code you want to profile

 * 2 : Life and death of a Profiling
	 Init profiler with a call to
	 YAP::Init(int maxPhases, int maxThreads, int maxEntries, int maxCallStack, void *(*allocMem)(size_t size) = malloc, void(*freeMem)(void* _memory) = free)
	 Memory is allocated once. maxPhases and maxThreads depend on your application. 
	 maxEntries is the maximum push/pop couples you'll have during a phase.
	 maxCallStack is the max callstack depth between a push and a pop section
	 You can also specify memory allocator.

	 Start a Phase with
	 YAP::PushPhase("GameInitExample");
	 Stop it with
	 YAP::PopPhase();

	 Inside a Phase, profile a section of code using:
	 unsigned int CodeSectionColor = 0xFFFFFFFF; //white
	 YAP::PushSection(CodeSection, CodeSectionColor);
	 or 
	 YAP::PushSection(CodeSection);
	 // ...code
	 YAP::PopSection();

	 You can also use a scoped section
	 {
	 YAP::ScopedSection(CodeSection, CodeSectionColor);
	 ....
	 // code
	 ...
	 }
	 When exiting, call YAP::Finish();

	 Call NewFrame when a new frame occurs.

	 Check Sample() function
 * 2.5 : Threading

	 in your threaded code, call
	 YAP::InitThread();
	 And 
	 YAP::FinishThread();
	 when your thread if terminating

	 InitThread/FinishThread, PushSection/PopSection are thread safe
	 Init/Finish, NewFrame, PushPhase/PopPhase are not

 * 3 : Logs and ImGui
 	You can see a ImGui log dumper calling:
	static bool profilerOpened(true);
	static bool profilerPaused(false);
	YAP::ImGuiLogger(&profilerOpened, &profilerPaused);

 * 4 : Credits
	Profiler by Cedric Guillemet @skaven_ cedric.guillemet@gmail.com
	ImGui by Omar Cornut @ocornut
	Os def & macro by Maxime Houlier
*/

#ifdef _WIN32
#include <Windows.h>
#endif

namespace YAP
{

	enum Colors
	{
		InitBase = 0x000655,
		InitMesh = 0x151d80,
		InitTexture = 0x3941aa,
		InitGame = 0x6a72d4,
		InitWindow= 0xaab0ff,
		InitShaders = 0x151d80,
		InitAudio = 0x3941aa,
		InitGUI = 0xaab0ff,
		InitRender = 0x000655,

		LoadMap = 0x002755,
		Load = 0x154580,
		BuildComponents = 0x396caa,

		Nav = 0x6a9ad4,
		NavmeshCompute = 0x9e5d62,
		NavpathCompute = 0x8dc382,

		Render = 0x333300,
		RenderBuildQueue = 0x4d4d0d,
		RenderShadows = 0x666622,
		RenderPostprocess = 0x7f7f40,
		RenderGBuffer = 0x999966,
		RenderSwapBuffer = 0x333300,

		GUI = 0x0b4100,
		GUIQueue = 0x1e6210,
		GUIComponent = 0x3a822b,
		GUIRender = 0x5fa351, 

		Game = 0x460633,
		GameAI = 0x63194d,
		GameProcedural = 0x7b3065,
		GameComponents = 0x975282,
		GameSound = 0xb680a6,
		GamePhysics = 0x460633,

		World = 0xd90d12,
		WorldPhysics = 0x682227,
		WorldCharacter = 0x813a3f, // remaining : 9e5d62,bd8a8e


		//remaining:
		// 1d055f, 3a1e87,5a3da8,866bcd,b5a2eb
		Misc = 0x606060,
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// OS definition

#define PLATFORM_OS_WINDOWS    1
#define PLATFORM_OS_LINUX      2
#define PLATFORM_OS_MACOSX     3

	//NOTE: all predefine C/C++ compiler macros: http://sourceforge.net/apps/mediawiki/predef/

#if defined( __WIN32__ ) || defined( _WIN32 ) || defined( __WIN64__ ) || defined( _WIN64 ) || defined( WIN32 )
#   define IS_OS_WINDOWS    1
#   define IS_OS_LINUX      0
#   define IS_OS_MACOSX     0
#   define PLATFORM_OS      PLATFORM_OS_WINDOWS
	//#   pragma message("Platform OS is Windows.")
#elif defined(__linux__) || defined( LINUX )
#   define IS_OS_WINDOWS    0
#   define IS_OS_LINUX      1
#   define IS_OS_MACOSX     0
#   define PLATFORM_OS      PLATFORM_OS_LINUX
#   pragma message("Platform OS is Linux.")
#elif ( defined(__APPLE__) && defined(__MACH__) )  || defined( MACOSX )
#   define IS_OS_WINDOWS    0
#   define IS_OS_LINUX      0
#   define IS_OS_MACOSX     1
#   define PLATFORM_OS      PLATFORM_OS_MACOSX
#   pragma message("Platform OS is MacOSX.")
#else
#   error "This platform is not supported."
#endif


#define PLATFORM_COMPILER_MSVC  1
#define PLATFORM_COMPILER_GCC   2

#if defined( _MSC_VER )
#   define PLATFORM_COMPILER            PLATFORM_COMPILER_MSVC
#   define PLATFORM_COMPILER_VERSION    _MSC_VER
#   define IS_COMPILER_MSVC     1
#   define IS_COMPILER_GCC      0
	//#   pragma message("Platform Compiler is Microsoft Visual C++.")
#elif defined( __GNUC__ )
#   define PLATFORM_COMPILER            PLATFORM_COMPILER_GCC
#   define PLATFORM_COMPILER_VERSION    (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#   define IS_COMPILER_MSVC     0
#   define IS_COMPILER_GCC      1
#   pragma message("Platform Compiler is GCC.")
#else
#   error "This compiler is not supported."
#endif


#define PLATFORM_MEMORY_ADDRESS_SPACE_32BIT  1
#define PLATFORM_MEMORY_ADDRESS_SPACE_64BIT  2

#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__)
#   define IS_PLATFORM_64BIT                1
#   define IS_PLATFORM_32BIT                0
#   define PLATFORM_MEMORY_ADDRESS_SPACE    PLATFORM_MEMORY_ADDRESS_SPACE_64BIT
	//#   pragma message("Using 64bit memory address space.")
#else
#   define IS_PLATFORM_64BIT                0
#   define IS_PLATFORM_32BIT                1
#   define PLATFORM_MEMORY_ADDRESS_SPACE    PLATFORM_MEMORY_ADDRESS_SPACE_32BIT
	//#   pragma message("Using 32bit memory address space.")
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Global vars
#ifdef YAP_IMPL
	int gPhaseCount = 0;
	int gPhaseIndex = 0;
	int gMaxThreads = 0;
	int gMaxEntries = 0;
	int gMaxPhases = 0;
	bool gPaused = false;
	bool gProfilerPaused = false;
#ifndef YAP_ENABLE
	int gTLSThreadIndex = 0;
#endif
#endif
	extern int gPhaseCount;
	extern int gPhaseIndex;
	extern int gMaxThreads;
	extern int gMaxEntries;
	extern int gMaxPhases;
	extern bool gPaused;
#ifndef YAP_ENABLE
	extern int gTLSThreadIndex;
#endif
	extern bool gProfilerPaused;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Structures

	struct ProfilerEntry
	{
		// static infos
		const char *szSection;
		const char *szFile;
		int line;
		unsigned int color;
		// infos used for rendering infos
		int level;
		double startTime;
		double endTime;
	};

	struct PhaseEntry
	{
		const char *szPhaseName;
		int frameWrite;
	};

	struct ThreadEntry
	{
		bool initialized;
		int entriesCount;
		int callStackDepth;
		int maxLevel;
		double minTime;
		double maxTime;
		unsigned int *colorStack;
		int *entriesStack;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Disable YAP 

#ifdef YAP_ENABLE

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// YAP is enabled

#if IS_OS_WINDOWS
	// Create A Structure For The Timer Information
	struct timer_t
	{
		__int64			frequency;									// Timer Frequency
		float			resolution;									// Timer Resolution
		unsigned long	mm_timer_start;								// Multimedia Timer Start Value
		unsigned long	mm_timer_elapsed;							// Multimedia Timer Elapsed Time
		bool			performance_timer;							// Using The Performance Timer?
		__int64			performance_timer_start;					// Performance Timer Start Value
		__int64			performance_timer_elapsed;					// Performance Timer Elapsed Time
	} ;														// Structure Is Named timer
#ifdef YAP_IMPL	
	timer_t timer;
#else
	extern timer_t timer;
#endif
#endif  //  IS_OS_WINDOWS

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Query timers

#if IS_OS_WINDOWS
	// Initialize Our Timer (Get It Ready)
	inline void TimerInit()
	{
		memset(&timer, 0, sizeof(timer));

		// Check to see if a performance counter is available
		// If one is available the timer frequency will be updated
		if (QueryPerformanceFrequency((LARGE_INTEGER *)&timer.frequency))
		{
			// Performance counter is available, use it instead of the multimedia timer
			// Get the current time and store it in performance_timer_start
			QueryPerformanceCounter((LARGE_INTEGER *)&timer.performance_timer_start);
			timer.performance_timer = true;				// Set performance timer to true

			// Calculate the timer resolution using the timer frequency
			timer.resolution = (float)(((double)1.0f) / ((double)timer.frequency));

			// Set the elapsed time to the current time
			timer.performance_timer_elapsed = timer.performance_timer_start;
		}
	}

	// platform specific get hires times...
	inline double GetHighResolutionTimer()
	{
		__int64 time;

		// Are we using the performance timer?
		if (timer.performance_timer)
		{
			// Grab the current performance time
			QueryPerformanceCounter((LARGE_INTEGER *)&time);

			// Return the current time minus the start time multiplied
			// by the resolution and 1000 (To Get MS)
			return ((double)(time - timer.performance_timer_start) * timer.resolution)*1000.0f;
		}
		return 0.0;
	}

#elif IS_OS_MACOSX
	// Initialize Our Timer (Get It Ready)
	inline void TimerInit()	{}

	inline double GetHighResolutionTimer()
	{
		UnsignedWide t;
		Microseconds(&t);
		/*time[0] = t.lo;
		time[1] = t.hi;
		*/
		double ms = double(t.hi * 1000LU);
		ms += double(t.lo / 1000LU);//*0.001;

		return ms;
	}

#else
	// Initialize Our Timer (Get It Ready)
	inline void TimerInit()	{}

	inline double GetHighResolutionTimer()
	{
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts); // Works on Linux

		double ms = double(ts.tv_sec * 1000LU);
		ms += double(ts.tv_nsec / 1000LU)*0.001;

		return ms;
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Push/pop macros

	// Phase
#define PushPhase(x) PushPhaseBase( #x )

	// Section
#define PushSection_1(x) PushSectionBase( #x, 0xFFFFFFFF, __FILE__, __LINE__ )
#define PushSection_2(x,y) PushSectionBase( #x, y, __FILE__, __LINE__ )
#define PushSection_X(x,y,z,...) z

#define PushSection_FUNC_RECOMPOSER(argsWithParentheses) PushSection_X argsWithParentheses
#define PushSection_CHOOSE_FROM_ARG_COUNT(...) PushSection_FUNC_RECOMPOSER((__VA_ARGS__, PushSection_2, PushSection_1, ))
#define PushSection_MACRO_CHOOSER(...) PushSection_CHOOSE_FROM_ARG_COUNT(__VA_ARGS__ ())
#define PushSection(...) PushSection_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)


	// Scoped Section
#define ScopedSection_1(x) ScopedSectionBase S##x__LINE__( #x, 0xFFFFFFFF, __FILE__, __LINE__ )
#define ScopedSection_2(x,y) ScopedSectionBase S##x__LINE__( #x, y, __FILE__, __LINE__ )
#define ScopedSection_X(x,y,z,...) z

#define ScopedSection_FUNC_RECOMPOSER(argsWithParentheses) ScopedSection_X argsWithParentheses
#define ScopedSection_CHOOSE_FROM_ARG_COUNT(...) ScopedSection_FUNC_RECOMPOSER((__VA_ARGS__, ScopedSection_2, ScopedSection_1, ))
#define ScopedSection_MACRO_CHOOSER(...) ScopedSection_CHOOSE_FROM_ARG_COUNT(__VA_ARGS__ ())
#define ScopedSection(...) ScopedSection_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)


	///////////////////////////////////////////////////////////////////////////////////////////////////
	// critical section

#if IS_OS_WINDOWS
	typedef CRITICAL_SECTION ZCriticalSection_t;
#elif IS_OS_LINUX
#include <pthread.h>
	typedef pthread_mutex_t ZCriticalSection_t;
#elif IS_OS_MACOSX
#import <CoreServices/CoreServices.h>
	typedef MPCriticalRegionID ZCriticalSection_t;
#endif

	__inline ZCriticalSection_t* NewCriticalSection()
	{
#if IS_OS_LINUX
		ZCriticalSection_t *cs = new pthread_mutex_t;
		//(*cs) = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_init(cs, NULL);
		return cs;
#elif IS_OS_MACOSX
		MPCriticalRegionID* criticalRegion = new MPCriticalRegionID;
		OSStatus err = MPCreateCriticalRegion(criticalRegion);
		if (err != 0)
		{
			delete criticalRegion;
			criticalRegion = NULL;
		}

		return criticalRegion;
#elif IS_OS_WINDOWS
		CRITICAL_SECTION *cs = new CRITICAL_SECTION;
		InitializeCriticalSection(cs);
		return cs;
#endif
	}

	__inline void DestroyCriticalSection(ZCriticalSection_t *cs)
	{
#if IS_OS_LINUX
		delete cs;
#elif IS_OS_MACOSX
		MPDeleteCriticalRegion(*cs);
#elif IS_OS_WINDOWS
		DeleteCriticalSection(cs);
		delete cs;
#endif
	}

	__inline void LockCriticalSection(ZCriticalSection_t *cs)
	{
#if IS_OS_LINUX
		pthread_mutex_lock(cs);
#elif IS_OS_MACOSX
		MPEnterCriticalRegion(*cs, kDurationForever);
#elif IS_OS_WINDOWS
		EnterCriticalSection(cs);
#endif
	}

	__inline void UnLockCriticalSection(ZCriticalSection_t *cs)
	{
#if IS_OS_LINUX
		pthread_mutex_unlock(cs);
#elif IS_OS_MACOSX
		MPExitCriticalRegion(*cs);
#elif IS_OS_WINDOWS
		LeaveCriticalSection(cs);
#endif 
	}

	// Critical section
#ifdef YAP_IMPL
	ZCriticalSection_t	*gProfilerCriticalSection(NULL);
#else
	extern ZCriticalSection_t	*gProfilerCriticalSection;
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// TLS

#ifdef YAP_IMPL
#if IS_OS_WINDOWS
	__declspec(thread) int gTLSThreadIndex = -1;
#elif IS_OS_MACOSX
	__thread int gTLSThreadIndex = -1;
#else // linux
	__thread int gTLSThreadIndex = -1;
#endif
#else
#if IS_OS_WINDOWS
	extern __declspec(thread) int gTLSThreadIndex;
#elif IS_OS_MACOSX
	extern __thread int gTLSThreadIndex = -1;
#else // linux
	extern __thread int gTLSThreadIndex = -1;
#endif
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// global mem

#ifdef YAP_IMPL
	ProfilerEntry *gProfilerEntries = NULL;
	PhaseEntry *gPhaseEntries = NULL;
	ThreadEntry *gThreadEntries = NULL;
	void(*_freeMemFunction)(void*_memory);
#else
	extern ProfilerEntry *gProfilerEntries;
	extern PhaseEntry *gPhaseEntries;
	extern ThreadEntry *gThreadEntries;
	extern void(*_freeMemFunction)(void*_memory);
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Assert

#ifdef _DEBUG
	inline void Assert(bool eval, const char * message)
	{
		if (!eval)
		{
#if IS_OS_WINDOWS
			MessageBoxA(NULL, message, "YAP assertion", MB_OK);
			DebugBreak();
#else
			raise(SIGTRAP);
#endif
		}
	}
#else
	inline void Assert(bool /*eval*/ , const char * /*message*/)
	{
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Profiler work

	inline void Init(int maxPhases, int maxThreads, int maxEntries, int maxCallStack, void *(*allocMem)(size_t size) = malloc, void(*freeMem)(void* _memory) = free)
	{
		Assert(gTLSThreadIndex == -1, "Main Thread is already initialized!");

		gPhaseEntries = (PhaseEntry*)allocMem(maxPhases * sizeof(PhaseEntry));
		gThreadEntries = (ThreadEntry*)allocMem(maxPhases * 2 * maxThreads * sizeof(ThreadEntry));

		int allocatedEntrySize = maxPhases * 2 * maxThreads * maxEntries * sizeof(ProfilerEntry);
		gProfilerEntries = (ProfilerEntry*)allocMem(allocatedEntrySize);

		for (int iZero = 0; iZero < maxPhases * 2 * maxThreads; iZero++)
		{
			ThreadEntry *threadEntry = &gThreadEntries[iZero];
			threadEntry->initialized = iZero == 0;
			threadEntry->entriesCount = 0;
			threadEntry->callStackDepth = 0;
			threadEntry->maxLevel = 0;
			threadEntry->minTime = 1.7976931348623158e+308;
			threadEntry->maxTime = 0.0;
			
			threadEntry->entriesStack = (int*)allocMem(maxCallStack * sizeof(int));
			threadEntry->colorStack = (unsigned int*)allocMem(maxCallStack * sizeof(unsigned int));
			for (int iColor = 0; iColor < maxCallStack;iColor++)
				threadEntry->colorStack[iColor] = 0x80808080;
		}

		gMaxThreads = maxThreads;
		gMaxEntries = maxEntries;
		_freeMemFunction = freeMem;
		gMaxPhases = maxPhases;
		gPhaseCount = 0;
		gPhaseIndex = -1;
		gTLSThreadIndex = 0;
		gThreadEntries[0].initialized = true;
		TimerInit();
		gProfilerCriticalSection = NewCriticalSection();
	}

	inline void InitThread()
	{
		LockCriticalSection(gProfilerCriticalSection);

		Assert(gTLSThreadIndex == -1, "Thread is already initialized!");

		for (int iThread = 0; iThread <gMaxThreads; iThread++)
		{
			ThreadEntry *threadEntry = &gThreadEntries[iThread];
			if (!threadEntry->initialized)
			{
				// use it
				gTLSThreadIndex = iThread;
				threadEntry->initialized = true;
				UnLockCriticalSection(gProfilerCriticalSection);
				return;
			}
		}
		Assert(false,"Every thread slots are used!"); // 
	}

	inline void FinishThread()
	{
		LockCriticalSection(gProfilerCriticalSection);
		Assert(gTLSThreadIndex > -1, "Trying to finish an uninitilized thread.");
		gThreadEntries[gTLSThreadIndex].initialized = false;
		gTLSThreadIndex = -1;
		UnLockCriticalSection(gProfilerCriticalSection);
	}

	inline void Finish()
	{
		for (int iFree = 0; iFree < gMaxThreads * 2; iFree++)
		{
			_freeMemFunction(gThreadEntries[iFree].entriesStack);
			_freeMemFunction(gThreadEntries[iFree].colorStack);
		}

		_freeMemFunction(gProfilerEntries);
		_freeMemFunction(gPhaseEntries);
		_freeMemFunction(gThreadEntries);

		gProfilerEntries = NULL;
		gPhaseEntries = NULL;
		gThreadEntries = NULL;

		DestroyCriticalSection(gProfilerCriticalSection);
	}

	inline void PushPhaseBase(const char *szPhaseName = "unknownPhase")
	{
		if (gPaused)
			return;
		for (int iPhase = 0; iPhase < gPhaseCount; iPhase++)
		{
			if (!strcmp(gPhaseEntries[iPhase].szPhaseName, szPhaseName))
			{
				gPhaseIndex = iPhase;
				return;
			}
		}
		// not found
		gPhaseIndex = gPhaseCount;
		gPhaseCount++;
		Assert(gPhaseCount <= gMaxPhases, "Maximum simulteanous phase count reached!");
		// todo: bound check!
		// add for both frames
		PhaseEntry *phase = &gPhaseEntries[gPhaseIndex];
		phase->szPhaseName = szPhaseName;
		phase->frameWrite = 0;
	}

	inline ThreadEntry *GetWrittingThread()
	{
		int frameWrite = gPhaseEntries[gPhaseIndex].frameWrite;
		int threadIndex = (gPhaseIndex * gMaxThreads * 2) + (frameWrite * gMaxThreads) + gTLSThreadIndex;
		ThreadEntry *threadEntry(&gThreadEntries[threadIndex]);
		return threadEntry;
	}

	inline ThreadEntry *GetReadingThread(int phaseIndex, int threadIndex)
	{
		int frameReadIndex = (gPhaseEntries[phaseIndex].frameWrite + 1) & 1;
		int thread = (phaseIndex * gMaxThreads * 2) + (frameReadIndex * gMaxThreads) + threadIndex;
		ThreadEntry *threadEntry(&gThreadEntries[thread]);
		return threadEntry;
	}

	inline void PushSectionBase(const char *szSection, unsigned int color, const char *szFile, int line)
	{
		if (gPaused)
			return;

		int frameWrite = gPhaseEntries[gPhaseIndex].frameWrite;
		ThreadEntry *threadEntry(GetWrittingThread());

		int entryIndex = gPhaseIndex * 2 * gMaxThreads * gMaxEntries;
		entryIndex += frameWrite * gMaxThreads * gMaxEntries;
		entryIndex += gTLSThreadIndex * gMaxEntries;
		entryIndex += threadEntry->entriesCount;

		Assert(threadEntry->entriesCount < gMaxEntries, "Max profile entries reached. Did you call NewFrame? If so, increase the max entries count at init");

		ProfilerEntry * profilerEntry(&gProfilerEntries[entryIndex]);

		threadEntry->entriesStack[threadEntry->callStackDepth] = entryIndex;
		if (color == 0xFFFFFFFF)
			color = threadEntry->colorStack[threadEntry->callStackDepth];

		Assert(szFile != NULL,"No file string specified");
		Assert(szSection != NULL,"No section name specified");
		profilerEntry->color = color;
		profilerEntry->szFile = szFile;
		profilerEntry->szSection = szSection;
		profilerEntry->line = line;
		profilerEntry->endTime = profilerEntry->startTime = GetHighResolutionTimer();

		if (++threadEntry->callStackDepth >= threadEntry->maxLevel)
			threadEntry->maxLevel = threadEntry->callStackDepth;

		threadEntry->colorStack[threadEntry->callStackDepth] = color;

		profilerEntry->level = threadEntry->callStackDepth;

		threadEntry->entriesCount++;

		if (profilerEntry->startTime < threadEntry->minTime)
			threadEntry->minTime = profilerEntry->startTime;
		if (profilerEntry->startTime > threadEntry->maxTime)
			threadEntry->maxTime = profilerEntry->startTime;
	}

	inline void PopSection()
	{
		if (gPaused)
			return;

		ThreadEntry *threadEntry(GetWrittingThread());
		
		int entryIndex = threadEntry->entriesStack[--threadEntry->callStackDepth];
		if (entryIndex<0)
			return; // not enough stack to pop

		ProfilerEntry * profilerEntry(&gProfilerEntries[entryIndex]);

		profilerEntry->endTime = GetHighResolutionTimer();

		if (profilerEntry->endTime > threadEntry->maxTime)
			threadEntry->maxTime = profilerEntry->endTime;
	}

	inline void NewFrame( )
	{
		gPaused = gProfilerPaused;
		if (gPaused)
			return;
		++gPhaseEntries[gPhaseIndex].frameWrite &= 1;
		ThreadEntry *threadEntry(GetWrittingThread());
		threadEntry->entriesCount = 0;
		threadEntry->maxLevel = 0;
		threadEntry->callStackDepth = 0;
		threadEntry->minTime = 1.7976931348623158e+308;
		threadEntry->maxTime = 0.0;
		threadEntry->initialized = true;
	}

	inline void PopPhase()
	{
		if (gPaused)
			return;

		int& rw = gPhaseEntries[gPhaseIndex].frameWrite;
		++rw &= 1;
	}

	struct ScopedSectionBase
	{
		ScopedSectionBase(const char *szSection, unsigned int color, const char *szFile, int line)
		{
			PushSectionBase(szSection, color, szFile, line);
		}
		~ScopedSectionBase()
		{
			PopSection();
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// helpers for logging / report

	inline ThreadEntry *GetThreadEntry(int phaseIndex, int threadIndex)
	{
		ThreadEntry *threadEntry(GetReadingThread(phaseIndex, threadIndex));
		return threadEntry;
	}

	inline int GetEntryCount(int phaseIndex, int threadIndex)
	{
		return GetThreadEntry(phaseIndex, threadIndex)->entriesCount;
	}

	inline ProfilerEntry *GetEntries(int phaseIndex, int threadIndex)
	{
		Assert(phaseIndex < gPhaseCount, "No phase for this index");
		Assert(threadIndex < gMaxThreads, "No thread for this index");

		int frameWrite = gPhaseEntries[phaseIndex].frameWrite;
		int frameReadIndex = (frameWrite + 1) & 1;
		int firstEntryIndex = phaseIndex * 2 * gMaxThreads * gMaxEntries;
		firstEntryIndex += frameReadIndex * gMaxThreads * gMaxEntries;
		firstEntryIndex += threadIndex * gMaxEntries;

		ProfilerEntry * profilerEntry(&gProfilerEntries[firstEntryIndex]);
		return profilerEntry;
	}

	inline ProfilerEntry *GetEntry(int phaseIndex, int threadIndex, int entryIndex)
	{
		return &GetEntries(phaseIndex, threadIndex)[entryIndex];
	}

	inline void GetPhaseNames(char *destination, int maxLen)
	{
		int av(0);

		for (int iPhase = 0; iPhase < gPhaseCount; iPhase++)
		{
			const char *szPhaseName = gPhaseEntries[iPhase].szPhaseName;
			int phaseNameLen = strlen(szPhaseName);
			for (int iLen = 0; iLen <= phaseNameLen; iLen++)
			{
				destination[av++] = szPhaseName[iLen];
				if (av == maxLen - 2)
				{
					destination[av] = 0;
					destination[av + 1] = 0;
					return;
				}
			}
		}
		destination[av++] = 0;
	}

	inline const char *GetPhaseName(int phaseIndex)
	{
		if (phaseIndex < 0 || phaseIndex >= gPhaseCount)
			return NULL;
		return gPhaseEntries[phaseIndex].szPhaseName;
	}

#ifdef YAP_IMPL
	void LogDump(int(*Log)(const char *szFormat, ...))
	{
		if (!Log)
			return;
		for (int iPhase = 0; iPhase < YAP::gPhaseCount; iPhase++)
		{
			Log("Phase %s\n", YAP::GetPhaseName(iPhase));
			for (int iThread = 0; iThread < YAP::gMaxThreads; iThread++)
			{
				Log("Thread %d :\n\n", iThread);

				int entriesCount = YAP::GetEntryCount(iPhase, iThread);
				for (int iEntry = 0; iEntry < entriesCount; iEntry++)
				{
					YAP::ProfilerEntry *entry = YAP::GetEntry(iPhase, iThread, iEntry);

					int iLevel = (entry->level >= 3) ? 3 : entry->level;
					static const char *indents[] = { "  ", "    ", "      ", "        " };
					Log("%s %s - %s(%d) %5.3f ms( %5.3f ms to %5.3f ms )\n",
						indents[iLevel],
						entry->szSection,
						entry->szFile,
						entry->line,
						(float)(entry->endTime - entry->startTime),
						(float)entry->startTime,
						(float)entry->endTime
						);
				}
				Log("---------------------------------------------------------------\n\n");
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Sample

	inline void CPULoadChild()
	{
		ScopedSection(CPULoadChild, 0x80808080);
		int v(0);
		for (int i = 0; i<300000; i++)
			v += i*i;
	}

	inline void CPULoad()
	{
		YAP::PushSection(CPULoad, 0x80808080);
		int v(0);
		for (int i = 0; i < 3000000; i++)
			v += i*i;;

		for (int i = 0; i < 4; i++)
			CPULoadChild();

		YAP::PopSection();
	}

	void Sample(int(*Log)(const char *szFormat, ...) = NULL)
	{
		// 2 phases max 
		// 4 threads
		// 2048 log entries per frame, per thread & per phase
		// 16 levels max callstack
		// default malloc/free
		YAP::Init(2, 4, 2048, 16);// , malloc, free); 

		YAP::PushPhase(LoadingPhase);
		YAP::PushSection(Loading);
		CPULoad();
		YAP::PopSection();
		YAP::PopPhase(); // LoadingPhase

		YAP::PushPhase(GamePhase);
		YAP::PushSection(MainSection);
		for (int iFrame = 0; iFrame < 5; iFrame++)
		{
			YAP::NewFrame();//swap frame
			YAP::PushSection(Frame);
			CPULoad();
			YAP::PopSection(); // Frame
		}

		YAP::PopSection(); // MainSection
		YAP::PopPhase(); // defaultPhase

		// log using function like printf
		YAP::LogDump(Log);

		YAP::Finish();
	}

#else // YAP_IMPL
	void LogDump(int(* /*Log*/)(const char *szFormat, ...));
	void Sample();
	void ImGuiLogger(bool* /*opened*/);
#endif // YAP_IMPL
#else
	// no YAP
	inline void Init(int, int, int, int, void *(*)(size_t) = NULL, void(*)(void*) = NULL) {}


	inline void InitThread() {}
	inline void FinishThread() {}
	inline void Finish() {}
	inline void PushPhase(const char *) {}
	inline ThreadEntry *GetWrittingThread() { return NULL; }
	inline ThreadEntry *GetReadingThread(int, int) { return NULL; }

	inline void PushSectionBase(const char *, unsigned int, const char *, int) {}


	inline void PopSection() {}
	inline void NewFrame( bool /*pasued*/) {}

	inline void PopPhase() {}

	struct ScopedSectionBase
	{
		ScopedSectionBase(const char *, unsigned int, const char *, int) {}
		~ScopedSectionBase() {}
	};

	// helpers for logging / report

	inline ThreadEntry *GetThreadEntry(int /*phaseIndex*/, int /*threadIndex*/) { return NULL; }
	inline int GetEntryCount(int /*phaseIndex*/, int /*threadIndex*/) { return 0; }
	inline ProfilerEntry *GetEntries(int /*phaseIndex*/, int /*threadIndex*/) { return NULL; }
	inline ProfilerEntry *GetEntry(int /*phaseIndex*/, int /*threadIndex*/, int /*entryIndex*/) { return NULL; }
	inline void GetPhaseNames(char * destination, int /*maxLen*/) { destination[0] = destination[1] = 0; }

	inline const char *GetPhaseName(int /*phaseIndex*/) { return NULL; }
	inline void LogDump(int(* /*Log*/)(const char *szFormat, ...)) {}
	inline void Sample() {}

	// Phase
	inline void dummyFunc() {}
#define PushPhase(x) dummyFunc()

	// Section
#define PushSection_1(x) dummyFunc()
#define PushSection_2(x,y) dummyFunc()
#define PushSection_X(x,y,z,...) z

#define PushSection_FUNC_RECOMPOSER(argsWithParentheses) PushSection_X argsWithParentheses
#define PushSection_CHOOSE_FROM_ARG_COUNT(...) PushSection_FUNC_RECOMPOSER((__VA_ARGS__, PushSection_2, PushSection_1, ))
#define PushSection_MACRO_CHOOSER(...) PushSection_CHOOSE_FROM_ARG_COUNT(__VA_ARGS__ ())
#define PushSection(...) PushSection_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)


	// Scoped Section
#define ScopedSection_1(x) dummyFunc()
#define ScopedSection_2(x,y) dummyFunc()
#define ScopedSection_X(x,y,z,...) z

#define ScopedSection_FUNC_RECOMPOSER(argsWithParentheses) ScopedSection_X argsWithParentheses
#define ScopedSection_CHOOSE_FROM_ARG_COUNT(...) ScopedSection_FUNC_RECOMPOSER((__VA_ARGS__, ScopedSection_2, ScopedSection_1, ))
#define ScopedSection_MACRO_CHOOSER(...) ScopedSection_CHOOSE_FROM_ARG_COUNT(__VA_ARGS__ ())
#define ScopedSection(...) ScopedSection_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#endif // YAP_ENABLE


	///////////////////////////////////////////////////////////////////////////////////////////////////
	// IMGUI
#ifdef YAP_IMPL
#ifdef YAP_IMGUI

	inline ImVec4 ImGuiColor(unsigned int v)
	{
		ImVec4 color;
		color.w = (float)((v & 0xFF000000) >> 24) * (1.f / 255.f);
		color.z = (float)((v & 0xFF0000) >> 16) * (1.f / 255.f);
		color.y = (float)((v & 0xFF00) >> 8) * (1.f / 255.f);
		color.x = (float)((v & 0xFF)) * (1.f / 255.f);
		return color;
	}

	void ImGuiLogger(bool* opened)
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiSetCond_FirstUseEver);
		if (!ImGui::Begin("Profiler", opened))
		{
			ImGui::End();
			return;
		}

		static int currentPhaseIndex = 1;
		char tmps[1024];
		YAP::GetPhaseNames(tmps, 1024);
		ImGui::PushItemWidth(200);
		ImGui::Combo("Phase", &currentPhaseIndex, tmps);

		ImGui::SameLine();
		static int timeRange = 0;
		//ImGui::PushItemWidth(-100);
		ImGui::Combo("Time range", &timeRange, "Fit All\0 30Hz\0 60Hz\0 120Hz\0\0");
		ImGui::SameLine();


		if (ImGui::Button("Copy output to clipboard"))
		{
			ImGui::LogToClipboard();
			YAP::LogDump((int(*)(const char *szFormat, ...))&ImGui::LogText);
			ImGui::LogFinish();
		}
		ImGui::SameLine();
		if (ImGui::Button(gProfilerPaused ? "Resume" : "Pause"))
			gProfilerPaused = !gProfilerPaused;

		ImGui::PopItemWidth();


		// get min/max
		double threadMinTime = FLT_MAX;
		double threadMaxTime = -FLT_MAX;
		for (int iThread = 0; iThread < YAP::gMaxThreads; iThread++)
		{
			YAP::ThreadEntry *te = YAP::GetThreadEntry(currentPhaseIndex, iThread);
			if (!te->initialized)
				continue;

			threadMinTime = (threadMinTime<te->minTime)?threadMinTime:te->minTime;
			threadMaxTime = (threadMaxTime>te->maxTime)?threadMaxTime:te->maxTime;
		}

		// GUI
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1)); // Tighten spacing
		for (int iThread = 0; iThread < YAP::gMaxThreads; iThread++)
		{
			ImGui::Text("Thread %d", iThread);
			ImGui::Separator();

			YAP::ThreadEntry *te = YAP::GetThreadEntry(currentPhaseIndex, iThread);
			if (!te->initialized)
				continue;

			float width = ImGui::GetWindowWidth() - 32;
			float scale(width / ((timeRange == 0) ? (float)(threadMaxTime - threadMinTime) : (33.333f / (float)timeRange)));

			for (int iLevel = 0; iLevel <= te->maxLevel; iLevel++)
			{
				int entriesCount = YAP::GetEntryCount(currentPhaseIndex, iThread);
				if (!entriesCount)
					continue;

				// count entries in level
				int entriesInLevelCount(0);
				for (int iEntry = 0; iEntry < entriesCount; iEntry++) // reversed order
				{
					YAP::ProfilerEntry *entry = YAP::GetEntry(currentPhaseIndex, iThread, iEntry);
					if (entry->level == iLevel)
						entriesInLevelCount++;
				}
				if (!entriesInLevelCount)
					continue;

				double currentTime = threadMinTime;
				ImGui::BeginGroup();
				for (int iEntry = 0; iEntry < entriesCount; iEntry++) // reversed order
				{
					YAP::ProfilerEntry *entry = YAP::GetEntry(currentPhaseIndex, iThread, iEntry);

					if (entry->level != iLevel)
						continue;

					float preTime = (float)(entry->startTime - currentTime);
					float invisibleButWidth = preTime * scale;
					if (invisibleButWidth >= 1.f)
					{
						ImGui::InvisibleButton("", ImVec2(invisibleButWidth, 20));
						ImGui::SameLine();
					}

					float duration = (float)(entry->endTime - entry->startTime);
					ImVec4 buttonColor(ImGuiColor(entry->color+0xFF000000));
					ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
					float butWidth = duration * scale;
					if (butWidth >= 1.f)
					{
						ImGui::Button(entry->szSection, ImVec2(butWidth, 20));
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("%s\n%s(%d)\nStart %5.3f ms\nEnd %5.3f ms\nDuration %5.3f ms",
								entry->szSection,
								entry->szFile,
								entry->line,
								(float)(entry->startTime - threadMinTime),
								(float)(entry->endTime - threadMinTime),
								(float)(entry->endTime - entry->startTime)
								);
						}
					}
					ImGui::PopStyleColor(3);
					ImGui::SameLine();
					currentTime = entry->endTime;
				}
				ImGui::EndGroup();
			}
		}
		ImGui::PopStyleVar();
		ImGui::End();
	}

#else // YAP_IMGUI
	void ImGuiLogger(bool* /*opened*/)
	{
	}
#endif // YAP_IMGUI
#else // YAP_IMPL
void ImGuiLogger(bool* /*opened*/);
#endif // YAP_IMPL

} // namespace YAP
