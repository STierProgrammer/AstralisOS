extern ringbuf_t key_ringbuf;
#define SMLIB_IMPLEMENTATION
#include <libs/smlib.h>

typedef struct model3d_t
{
    const vec3_t *vcs;
    size_t num_faces;
    const int (*fs)[3];
} model3d_t;

vec2_t to_surface_coordinate(gfx_surface_t *surface, vec2_t p)
{
    return (vec2_t){
        .x = (p.x + 1) / 2 * surface_height(surface),
        .y = (-p.y + 1) / 2 * surface_height(surface),
    };
}

void draw_line(gfx_surface_t *surface, vec2_t p1, vec2_t p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;

    int steps = (int)(abs(dx) > abs(dy) ? abs(dx) : abs(dy));

    if (steps == 0)
    {
        gfx_surface_put_pixel(surface, p1.x, p1.y, 0xFF);
        return;
    }
    
    float x_inc = dx / steps;
    float y_inc = dy / steps;

    float x = p1.x;
    float y = p1.y;

    for (int i = 0; i <= steps; i++)
    {
        gfx_surface_put_pixel(surface, (size_t)x, (size_t)y, 0xFFFF00); 
        x += x_inc;
        y += y_inc;
    }
}

void clear(gfx_surface_t *surface, uint32_t color)
{
    for (size_t i = 0; i < surface_height(surface); ++i)
    {
        for (size_t j = 0; j < surface_width(surface); ++j)
        {
            gfx_surface_put_pixel(surface, j, i, color);
        }
    }
}

static float angle = 0;
vec3_t camera = vec3(0, 0, 0);
void model3d_render(gfx_surface_t *surface, model3d_t *model)
{
    for (size_t i = 0; i < model->num_faces; ++i)
    {
        const int *f = model->fs[i];

        for (size_t j = 0; j < 3; ++j)
        {
            vec3_t p1 = model->vcs[f[j]];
            vec3_t p2 = model->vcs[f[(j + 1) % 3]];

    
            p1.x -= camera.x;
            p1.y -= camera.y;
            p1.z -= camera.z;

            p2.x -= camera.x;
            p2.y -= camera.y;
            p2.z -= camera.z;

            vec2_t rp1 = to_surface_coordinate(
                    surface, 
                    vec3_project_to_vec2(p1)
                    );

            vec2_t rp2 = to_surface_coordinate(
                    surface, 
                     vec3_project_to_vec2(
                            p2
                        )
       
                    );

            draw_line(surface, rp1, rp2);
        }
    }
}

void redner()
{
    gfx_surface_t *surface = gfx_surface_create(&bootctx(fbs)[0]);
    
    const int FPS = 60;
    const int fs[][3] = {
        {0, 1, 4},
        {1, 2, 4},
        {2, 3, 4},
        {3, 0, 4},
        {0, 2, 1},
        {0, 3, 2},
    };

    const vec3_t vcs[] = {
        vec3(0, 0, 0),
        vec3(0, 0, 1),
        vec3(1, 0, 1),
        vec3(1, 0, 0),
        vec3(0.5, 1, 0.5),
    };

    model3d_t model = {
        .vcs = vcs,
        .num_faces = 6,
        .fs = fs
    };

    const float dt = 1.0 / FPS;
    while (1)
    {
        kbd_ev_t ev;
        ringbuf_read(&key_ringbuf, &ev);
        if (ev.action == KEY_ACTION_PRESS)
        {
            switch (ev.keycode) {
                case KEY_W:
                {
                    camera.z += 0.1f;
                    break;
                }
                case KEY_S:
                {
                    camera.z -= 0.1f;
                    break;
                }
                case KEY_A:
                {
                    camera.x -= 0.1f;
                    break;
                }
                case KEY_D:
                {
                    camera.x += 0.1f;
                    break;
                }
                case KEY_SPACE:
                {
                    camera.y += 0.01f;
                    break;
                }
                case KEY_LEFT_SHIFT:
                {
                    camera.y -= 0.01f;
                    break;
                }
                default: {}
            }
        }

        angle += dt * HALF_PI;
        clear(surface, 0);
        model3d_render(surface, &model);
        gfx_surface_sync(surface, 0, surface_height(surface) * surface_width(surface) - 1);
        
    }
}

