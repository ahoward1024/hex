/* Xiaolin Wu's line algorithm, based on Michael Abrash's implementation */
#define WULINE(x1, y1, x2, y2, opaque_op, blend_op, draw_end) \
{ \
    Uint16 ErrorAdj, ErrorAcc; \
    Uint16 ErrorAccTemp, Weighting; \
    int DeltaX, DeltaY, Temp, XDir; \
    unsigned r, g, b, a, inva; \
 \
    /* Draw the initial pixel, which is always exactly intersected by \
       the line and so needs no weighting */ \
    opaque_op(x1, y1); \
 \
    /* Draw the final pixel, which is always exactly intersected by the line \
       and so needs no weighting */ \
    if (draw_end) { \
        opaque_op(x2, y2); \
    } \
 \
    /* Make sure the line runs top to bottom */ \
    if (y1 > y2) { \
        Temp = y1; y1 = y2; y2 = Temp; \
        Temp = x1; x1 = x2; x2 = Temp; \
    } \
    DeltaY = y2 - y1; \
 \
    if ((DeltaX = x2 - x1) >= 0) { \
        XDir = 1; \
    } else { \
        XDir = -1; \
        DeltaX = -DeltaX; /* make DeltaX positive */ \
    } \
 \
    /* line is not horizontal, diagonal, or vertical */ \
    ErrorAcc = 0;  /* initialize the line error accumulator to 0 */ \
 \
    /* Is this an X-major or Y-major line? */ \
    if (DeltaY > DeltaX) { \
        /* Y-major line; calculate 16-bit fixed-point fractional part of a \
          pixel that X advances each time Y advances 1 pixel, truncating the \
          result so that we won't overrun the endpoint along the X axis */ \
        ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY; \
        /* Draw all pixels other than the first and last */ \
        while (--DeltaY) { \
            ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */ \
            ErrorAcc += ErrorAdj;      /* calculate error for next pixel */ \
            if (ErrorAcc <= ErrorAccTemp) { \
                /* The error accumulator turned over, so advance the X coord */ \
                x1 += XDir; \
            } \
            y1++; /* Y-major, so always advance Y */ \
            /* The IntensityBits most significant bits of ErrorAcc give us the \
             intensity weighting for this pixel, and the complement of the \
             weighting for the paired pixel */ \
            Weighting = ErrorAcc >> 8; \
            { \
                a = DRAW_MUL(_a, (Weighting ^ 255)); \
                r = DRAW_MUL(_r, a); \
                g = DRAW_MUL(_g, a); \
                b = DRAW_MUL(_b, a); \
                inva = (a ^ 0xFF); \
                blend_op(x1, y1); \
            } \
            { \
                a = DRAW_MUL(_a, Weighting); \
                r = DRAW_MUL(_r, a); \
                g = DRAW_MUL(_g, a); \
                b = DRAW_MUL(_b, a); \
                inva = (a ^ 0xFF); \
                blend_op(x1 + XDir, y1); \
            } \
        } \
    } else { \
        /* X-major line; calculate 16-bit fixed-point fractional part of a \
           pixel that Y advances each time X advances 1 pixel, truncating the \
           result to avoid overrunning the endpoint along the X axis */ \
        ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX; \
        /* Draw all pixels other than the first and last */ \
        while (--DeltaX) { \
            ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */ \
            ErrorAcc += ErrorAdj;      /* calculate error for next pixel */ \
            if (ErrorAcc <= ErrorAccTemp) { \
                /* The error accumulator turned over, so advance the Y coord */ \
                y1++; \
            } \
            x1 += XDir; /* X-major, so always advance X */ \
            /* The IntensityBits most significant bits of ErrorAcc give us the \
              intensity weighting for this pixel, and the complement of the \
              weighting for the paired pixel */ \
            Weighting = ErrorAcc >> 8; \
            { \
                a = DRAW_MUL(_a, (Weighting ^ 255)); \
                r = DRAW_MUL(_r, a); \
                g = DRAW_MUL(_g, a); \
                b = DRAW_MUL(_b, a); \
                inva = (a ^ 0xFF); \
                blend_op(x1, y1); \
            } \
            { \
                a = DRAW_MUL(_a, Weighting); \
                r = DRAW_MUL(_r, a); \
                g = DRAW_MUL(_g, a); \
                b = DRAW_MUL(_b, a); \
                inva = (a ^ 0xFF); \
                blend_op(x1, y1 + 1); \
            } \
        } \
    } \
}

#ifdef AA_LINES
#define AALINE(x1, y1, x2, y2, opaque_op, blend_op, draw_end) \
            WULINE(x1, y1, x2, y2, opaque_op, blend_op, draw_end)
#else
#define AALINE(x1, y1, x2, y2, opaque_op, blend_op, draw_end) \
            BLINE(x1, y1, x2, y2, opaque_op, draw_end)
#endif