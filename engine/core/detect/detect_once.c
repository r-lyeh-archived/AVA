#pragma once

#define once_unique(a) once_join(a, __LINE__)
#define once_join(a,b) once_jo1n(a,b)
#define once_jo1n(a,b) a##b

#define ONCE \
    static int once_unique(once) = 0; for(; !once_unique(once); once_unique(once) = 1)

/*
int main() {
    for( int i = 0; i < 10; ++i) {
        ONCE {
            puts("hi1");
        }
        ONCE {
            puts("hi2");
        }
    }
}
*/
