#include <engine.h>

int main() {
    bool frustum_cull_enabled = 1;
    int objects_total = 0;
    int objects_rendered = 0;

    window_create( 0.50f, 0 );

    ddraw_color3(vec3(0.5,0.5,0.5));

    camera cam;
    camera_create(&cam, 0.25f, false, true);
    camera_teleport(&cam, vec3(0,0,0), vec2(0,-15));

    while( window_update() ) {
        bool active = !!mouse('L') || !!mouse('R');
        set_mouse(active ? 'hide' : 'show');
        camera_enable( &cam, active );
        camera_fps( &cam, key_wasdec(), mouse_xy() );

        mat44 proj; perspective44(proj, 45, window_aspect(), 0.1f, 1000);
        mat44 view; copy44(view, cam.view);
        mat44 projview; multiply44(projview, proj, view);

        objects_total = 0;
        objects_rendered = 0;

        if( key_down(' ') ) frustum_cull_enabled ^= 1;

        frustum f = frustum_build(projview);

        ddraw_begin(projview);
        for(int i = -300; i < 300; i += 5) {
            for(int j =  -300; j < 300; j += 5) {
                objects_total++;

                vec3 minpos = vec3(i,-10,j);
                vec3 maxpos = add3(minpos, vec3(2.5,2.5,2.5));

                bool visible = frustum_test_aabb(f, aabb(minpos, maxpos));

                if ( frustum_cull_enabled && !visible ) {
                    // skip rendering
                    continue;
                }

                ddraw_aabb(minpos, maxpos);
                objects_rendered++;
            }
        }
        ddraw_end();

        ddraw_printf(window_stats());
        ddraw_printf("frustum_cull_enabled:%s objects_rendered:%d/%d", frustum_cull_enabled ? "on":"off", objects_rendered, objects_total);
        ddraw_printf("space - toggle frustum culling on/off");
        window_swap(0);
    }
}
