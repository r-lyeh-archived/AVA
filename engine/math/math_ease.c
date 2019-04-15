// easing functions. original code by @krig (Kristoffer Gronlund) (PD licensed)
//-rlyeh, public domain.

#ifndef EASE_H
#define EASE_H
#include <math.h>

static m_inline float ease_linear(float t) { return t; }

static m_inline float ease_quadratic_in(float t) { return t*t; }
static m_inline float ease_quadratic_out(float t) { return -(t*(t-2)); }
static m_inline float ease_quadratic_inout(float t) { return (t < 0.5f) ? 2*t*t : (-2*t*t)+(4*t)-1; }

static m_inline float ease_cubic_in(float t) { return t*t*t; }
static m_inline float ease_cubic_out(float t) { float f=t-1; return f*f*f+1; }
static m_inline float ease_cubic_inout(float t) { float f; return (t < 0.5f) ? 4*t*t*t : (f=(2*t)-2,0.5f*f*f*f+1); }

static m_inline float ease_quartic_in(float t) { return t*t*t*t; }
static m_inline float ease_quartic_out(float t) { float f=t-1; return f*f*f*(1-t)+1; }
static m_inline float ease_quartic_inout(float t) { float f; return (t < 0.5f) ? 8*t*t*t*t : (f=(t-1),-8*f*f*f*f+1); }

static m_inline float ease_quintic_in(float t) { return t*t*t*t*t; }
static m_inline float ease_quintic_out(float t) { float f=(t-1); return f*f*f*f*f+1; }
static m_inline float ease_quintic_inout(float t) { float f; return (t < 0.5f) ? 16*t*t*t*t*t : (f=((2*t)-2),0.5f*f*f*f*f*f+1); }

static m_inline float ease_sine_in(float t) { return 1+sinf((t-1)*(M__PI*0.5f)); }
static m_inline float ease_sine_out(float t) { return sinf(t*(M__PI*0.5f)); }
static m_inline float ease_sine_inout(float t) { return 0.5f*(1-cosf(t*M_PI)); }

static m_inline float ease_circular_in(float t) { return 1-sqrtf(1-(t*t)); }
static m_inline float ease_circular_out(float t) { return sqrtf((2-t)*t); }
static m_inline float ease_circular_inout(float t) { return t < 0.5f ? 0.5f*(1-sqrtf(1-4*(t*t))) : 0.5f*(sqrtf(-((2*t)-3)*((2*t)-1))+1); }

static m_inline float ease_exponential_in(float t) { return (t <= 0) ? t : powf(2,10*(t-1)); }
static m_inline float ease_exponential_out(float t) { return (t >= 1) ? t : 1-powf(2,-10*t); }
static m_inline float ease_exponential_inout(float t) { return (t <= 0 || t >= 1) ? t : t < 0.5f ? 0.5f*powf(2,(20*t)-10) : -0.5f*powf(2,(-20*t)+10)+1; }

static m_inline float ease_elastic_in(float t) { return sinf(13*(M__PI*0.5f)*t)*powf(2,10*(t-1)); }
static m_inline float ease_elastic_out(float t) { return sinf(-13*(M__PI*0.5f)*(t+1))*powf(2,-10*t)+1; }
static m_inline float ease_elastic_inout(float t) { return t < 0.5f ? 0.5f*sinf(13*(M__PI*0.5f)*(2*t))*powf(2,10*((2*t)-1)) : 0.5f*(sinf(-13*(M__PI*0.5f)*((2*t-1)+1))*powf(2,-10*(2*t-1))+2); }

static m_inline float ease_back_in(float t) { return t*t*t-t*sinf(t*M_PI); }
static m_inline float ease_back_out(float t) { float f=1-t; return 1-(f*f*f-f*sinf(f*M_PI)); }
static m_inline float ease_back_inout(float t) { float f; return t < 0.5f ? (f=2*t,0.5f*(f*f*f-f*sinf(f*M_PI))) : (f=(1-(2*t-1)),0.5f*(1-(f*f*f-f*sinf(f*M_PI)))+0.5f); }

static m_inline float ease_bounce_out(float t) { return (t < 4.f/11) ? (121.f*t*t)/16 : (t < 8.f/11) ? (363.f/40*t*t)-(99.f/10*t)+17.f/5 : (t < 9.f/10) ? (4356.f/361*t*t)-(35442.f/1805*t)+16061.f/1805 : (54.f/5*t*t)-(513.f/25*t)+268.f/25; }
static m_inline float ease_bounce_in(float t) { return 1-ease_bounce_out(1-t); }
static m_inline float ease_bounce_inout(float t) { return t < 0.5f ? 0.5f*ease_bounce_in(t*2) : 0.5f*ease_bounce_out(t*2-1)+0.5f; }

static m_inline float ease_perlin_inout(float t) { float t3=t*t*t,t4=t3*t,t5=t4*t; return 6*t5-15*t4+10*t3; }

#endif
