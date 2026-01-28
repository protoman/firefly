#ifndef ST_COLOR_HPP
#define ST_COLOR_HPP

struct st_color {
    int r;
    int g;
    int b;
    st_color () {
        r = -1;
        g = -1;
        b = -1;
    }

    st_color (int setR, int setG, int setB) {
        r = setR;
        g = setG;
        b = setB;
    }
};

#endif // ST_COLOR_HPP
