#include <engine.h>

int main() {
    window_create(0.50f, WINDOW_MSAA4);

    while( window_update()) {
        // projection
        mat44 proj;
        perspective44(proj, 45, window_aspect(), 0.1, 1000);

        // pan camera view
        static float f = 25; f += 0.1f; if(f > 50) f = -50;
        vec3 cam_pos = vec3( f, 50, 50);
        vec3 eye_pos = { 0, 0, 0 };
        vec3 up_vec = { 0, 1, 0 };
        mat44 view;
        lookat44(view, cam_pos, eye_pos, up_vec);

        // projview
        mat44 projview; 
        multiply44(projview, proj, view);

        // queue a few ddraws
        ddraw_begin(projview);

            const float radius = 5, height = 15;
            ddraw_grid(10,10,10);
            ddraw_aabb(vec3(-100,-10,-100), vec3(100,10,100));
            ddraw_axis(vec3(10,0,-10), radius);

            ddraw_line(vec3(-10,0,10), vec3(0,0,0));
            ddraw_normal(vec3(-20,0,20), vec3(0,1,0), radius);
            ddraw_cross(vec3(-30,0,30), radius);
            ddraw_sphere(vec3(-10,0,-10), radius);

            ddraw_box(vec3(-20,0,-20), vec3(radius,radius*2,radius*3));

            ddraw_cone(vec3(30,height,-30), vec3(30,0,-30), radius);
            ddraw_arrow(vec3(40,0,-40), vec3(40,height,-40));
            ddraw_ring(vec3(20,height,-20), vec3(0,1,0), radius);
            ddraw_circle(vec3(20,0,-20), vec3(0,1,0), radius);
            ddraw_sphere2(vec3(50,0,-50), radius);

            ddraw_printf("hello world");
            ddraw_text2d(vec2(window_width()/2,window_height()/2), "hello world 1");
            ddraw_text2d(vec2(window_width()/2+20,window_height()/2+20), "hello world 2");

            // The frustum will depict a fake camera:
            mat44 fakeproj; perspective44(fakeproj, 45, window_aspect(), 0.1, 10);
            mat44 fakeview; lookat44(fakeview, vec3(0,0,0), vec3(-10,1,-10), vec3(0,1,0));
            mat44 fakepv;   multiply44(fakepv, fakeproj, fakeview);
            ddraw_frustum(fakepv);

        ddraw_end();

        static int counter = 0;
        ddraw_console("console line test %d", counter++);

        ddraw_printf(window_stats());
        window_swap(0);
    }
}
