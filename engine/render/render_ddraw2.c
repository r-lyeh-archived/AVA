// original code by @vurtun (PD).
// [src] https://gist.github.com/vurtun/95f088e4889da2474ad1ce82d7911fee
// - rlyeh, public domain.

#ifndef DDRAW2_H
#define DDRAW2_H

API void ddColor(float r, float g, float b);
API void ddLine(vec3 a, vec3 b);
API void ddPlane(vec3 p, vec3 n, float scale);
API void ddTriangle(vec3 a, vec3 b, vec3 c);
API void ddArrow(vec3 from, vec3 to, const float size);
API void ddSphere(vec3 c, const float radius);
API void ddCapsule(vec3 from, vec3 to, float r);
API void ddPyramid(vec3 f, vec3 t, float size);
API void ddDiamond(vec3 from, vec3 to, float size);
API void ddGrid(float mins, float maxs, float y, float step);
API void ddBounds(const vec3 points[8]);
API void ddBox(vec3 c, vec3 whd);
API void ddAabb(const vec3 bounds[2]);

#endif
#ifdef DDRAW2_C
#pragma once

void ddColor(float r, float g, float b) {
    glColor3f(r,g,b);
}
void ddLine(vec3 a, vec3 b) {
    // @todo: replace me with shaders
    glBegin(GL_LINES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glEnd();
}
void ddPlane(vec3 p, vec3 n, float scale) {
    vec3 v1, v2, v3, v4;
    vec3 tangent, bitangent;

    ortho3(&tangent, &bitangent, n);
    #define DD_PLANE_V(v, op1, op2) \
        v.x = (p.x op1 (tangent.x*scale) op2 (bitangent.x*scale)); \
        v.y = (p.y op1 (tangent.y*scale) op2 (bitangent.y*scale)); \
        v.z = (p.z op1 (tangent.z*scale) op2 (bitangent.z*scale))

    DD_PLANE_V(v1,-,-);
    DD_PLANE_V(v2,+,-);
    DD_PLANE_V(v3,+,+);
    DD_PLANE_V(v4,-,+);
    #undef DD_PLANE_V

    ddLine(v1,v2);
    ddLine(v2,v3);
    ddLine(v3,v4);
    ddLine(v4,v1);
}
void ddTriangle(vec3 a, vec3 b, vec3 c) {
    ddLine(a,b);
    ddLine(b,c);
    ddLine(c,a);
}
void ddArrow(vec3 from, vec3 to, const float size) {
    int i = 0;
    float degrees = 0.0f;
    static const float arrow_step = 30.0f;
    static const float arrow_sin[45] = {
        0.0f, 0.5f, 0.866025f, 1.0f, 0.866025f, 0.5f, -0.0f, -0.5f, -0.866025f,
        -1.0f, -0.866025f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };
    static const float arrow_cos[45] = {
        1.0f, 0.866025f, 0.5f, -0.0f, -0.5f, -0.866026f, -1.0f, -0.866025f, -0.5f, 0.0f,
        0.5f, 0.866026f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };
    vec3 up, right, forward;

    forward = sub3(to, from);
    forward = norm3(forward);
    ortho3(&right, &up, forward);
    forward = scale3(forward, size);

    ddLine(from, to);
    for (i = 0; degrees < 360.0f; degrees += arrow_step, ++i) {
        float scale;
        vec3 v1, v2, temp;

        scale = 0.5f * size * arrow_cos[i];
        temp = scale3(right, scale);
        v1 = sub3(to, forward);
        v1 = add3(v1, temp);

        scale = 0.5f * size * arrow_sin[i];
        temp = scale3(up, scale);
        v1 = add3(v1, temp);

        scale = 0.5f * size * arrow_cos[i + 1];
        temp = scale3(right, scale);
        v2 = sub3(to, forward);
        v2 = add3(v2, temp);

        scale = 0.5f * size * arrow_sin[i + 1];
        temp = scale3(up, scale);
        v2 = add3(v2, temp);

        ddLine(v1, to);
        ddLine(v1, v2);
    }
}
void ddSphere(vec3 c, const float radius) {
    #define STEP_SIZE 15
    vec3 last, tmp;
    vec3 cache[(360 / STEP_SIZE)];
    int j = 0, i = 0, n = 0;

    cache[0] = vec3(c.x,c.y,c.z+radius);
    for (n = 1; n < count_of(cache); ++n)
        cache[n] = cache[0];

    /* first circle iteration */
    for (i = STEP_SIZE; i <= 360; i += STEP_SIZE) {
        const float sa = sinf(i*TO_RAD);
        const float ca = cosf(i*TO_RAD);

        last.x = c.x;
        last.y = c.y + radius * sa;
        last.z = c.z + radius * ca;

        /* second circle iteration */
        for (n = 0, j = STEP_SIZE; j <= 360; j += STEP_SIZE, ++n) {
            tmp.x = c.x + sinf(TO_RAD*j)*radius*sa;
            tmp.y = c.y + cosf(TO_RAD*j)*radius*sa;
            tmp.z = last.z;

            ddLine(last, tmp);
            ddLine(last, cache[n]);

            cache[n] = last;
            last = tmp;
        }
    }
    #undef STEP_SIZE
}
void ddCapsule(vec3 from, vec3 to, float r) {
    int i = 0;
    static const int step_size = 20;
    vec3 up, right, forward;
    vec3 lastf, lastt;

    /* calculate axis */
    forward = sub3(to, from);
    forward = norm3(forward);
    ortho3(&right, &up, forward);

    /* calculate first two cone verts (buttom + top) */
    lastf = scale3(up,r);
    lastt = add3(to,lastf);
    lastf = add3(from,lastf);

    /* step along circle outline and draw lines */
    for (i = step_size; i <= 360; i += step_size) {
        /* calculate current rotation */
        vec3 ax, ay, pf, pt, tmp;
        ax = scale3(right, sinf(i*TO_RAD));
        ay = scale3(up, cosf(i*TO_RAD));

        /* calculate current vertices on cone */
        tmp = add3(ax, ay);
        pf = scale3(tmp, r);
        pt = scale3(tmp, r);

        pf = add3(pf, from);
        pt = add3(pt, to);

        /* draw cone vertices */
        ddLine(lastf, pf);
        ddLine(lastt, pt);
        ddLine(pf, pt);

        lastf = pf;
        lastt = pt;

        /* calculate first top sphere vert */
        vec3 prevt, prevf;
        prevt = scale3(tmp, r);
        prevf = add3(prevt, from);
        prevt = add3(prevt, to);

        /* sphere (two half spheres )*/
        for (int j = 1; j < 180/step_size; j++) {
            /* angles */
            float ta = j*step_size;
            float fa = 360-(j*step_size);
            vec3 t;

            /* top half-sphere */
            ax = scale3(forward, sinf(ta*TO_RAD));
            ay = scale3(tmp, cosf(ta*TO_RAD));

            t = add3(ax, ay);
            pf = scale3(t, r);
            pf = add3(pf, to);
            ddLine(pf, prevt);
            prevt = pf;

            /* bottom half-sphere */
            ax = scale3(forward, sinf(fa*TO_RAD));
            ay = scale3(tmp, cosf(fa*TO_RAD));

            t = add3(ax, ay);
            pf = scale3(t, r);
            pf = add3(pf, from);
            ddLine(pf, prevf);
            prevf = pf;
        }
    }
}
void ddPyramid(vec3 f, vec3 t, float size) {
    poly p = pyramid(f, t, size);
    vec3 *pyr = p.verts;

    vec3 *a = pyr + 0;
    vec3 *b = pyr + 1;
    vec3 *c = pyr + 2;
    vec3 *d = pyr + 3;
    vec3 *r = pyr + 4;

    /* draw vertices */
    ddLine(*a, *b);
    ddLine(*b, *c);
    ddLine(*c, *d);
    ddLine(*d, *a);

    /* draw root */
    ddLine(*a, *r);
    ddLine(*b, *r);
    ddLine(*c, *r);
    ddLine(*d, *r);

    array_free(pyr);
}
void ddDiamond(vec3 from, vec3 to, float size) {
    poly p = diamond(from, to, size);
    vec3 *dmd = p.verts;

    vec3 *a = dmd + 0;
    vec3 *b = dmd + 1;
    vec3 *c = dmd + 2;
    vec3 *d = dmd + 3;
    vec3 *t = dmd + 4;
    vec3 *f = dmd + 5;

    /* draw vertices */
    ddLine(*a, *b);
    ddLine(*b, *c);
    ddLine(*c, *d);
    ddLine(*d, *a);

    /* draw roof */
    ddLine(*a, *t);
    ddLine(*b, *t);
    ddLine(*c, *t);
    ddLine(*d, *t);

    /* draw floor */
    ddLine(*a, *f);
    ddLine(*b, *f);
    ddLine(*c, *f);
    ddLine(*d, *f);

    array_free(dmd);
}
void ddGrid(float mins, float maxs, float y, float step) {
    float i;
    vec3 from, to;
    for (i = mins; i <= maxs; i += step) {
        /* Horizontal line (along the X) */
        from = vec3(mins,y,i);
        to = vec3(maxs,y,i);
        ddLine(from,to);
        /* Vertical line (along the Z) */
        from = vec3(i,y,mins);
        to = vec3(i,y,maxs);
        ddLine(from,to);
    }
}
void ddBounds(const vec3 points[8]) {
    int i = 0;
    for (i = 0; i < 4; ++i) {
        ddLine(points[i], points[(i + 1) & 3]);
        ddLine(points[i], points[4 + i]);
        ddLine(points[4 + i], points[4 + ((i + 1) & 3)]);
    }
}
void ddBox(vec3 c, vec3 whd) {
    vec3 pnts[8];
    whd = scale3(whd, 0.5f);

    #define DD_BOX_V(v, op1, op2, op3)\
        (v).x = c.x op1 whd.x; (v).y = c.y op2 whd.y; (v).z = c.z op3 whd.z
    DD_BOX_V(pnts[0], -, +, +);
    DD_BOX_V(pnts[1], -, +, -);
    DD_BOX_V(pnts[2], +, +, -);
    DD_BOX_V(pnts[3], +, +, +);
    DD_BOX_V(pnts[4], -, -, +);
    DD_BOX_V(pnts[5], -, -, -);
    DD_BOX_V(pnts[6], +, -, -);
    DD_BOX_V(pnts[7], +, -, +);
    #undef DD_BOX_V
    ddBounds(pnts);
}
void ddAabb(const vec3 bounds[2]) {
    int i = 0;
    vec3 bb[2], pnts[8];
    bb[0] = bounds[0];
    bb[1] = bounds[1];
    for (i = 0; i < count_of(pnts); ++i) {
        pnts[i].x = bb[(((i ^ (i >> 1)) & 1))].x;
        pnts[i].y = bb[(((i >> 1) & 1))].y;
        pnts[i].z = bb[(((i >> 2) & 1))].z;
    }
    ddBounds(pnts);
}

#endif
