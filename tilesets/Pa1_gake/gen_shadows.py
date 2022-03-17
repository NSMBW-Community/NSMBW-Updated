# 2022-03-16, RRWMC

import math

from PyQt6 import QtCore, QtGui

SCALE = 1


def clamp(x: float) -> float:
    """Clamps x to [0.0, 1.0]"""
    if x < 0:
        return 0
    elif x > 1:
        return 1
    else:
        return x


def ease_in_out_quad(x: float) -> float:
    """https://easings.net/#easeInOutQuad"""
    if x < 0.5:
        return 2 * x * x
    else:
        return 1 - ((-2 * x + 2) ** 2) / 2


def linear_scale_between(x1: float, x2: float, y1: float, y2: float, x: float, *, clamp=True, easing_func=None) -> float:
    """Standard linear interpolation function, optionally clamped (between y1 and y2)"""

    pct = (x - x1) / (x2 - x1)

    if easing_func:
        pct = easing_func(pct)

    if clamp:
        pct = globals()['clamp'](pct)

    return y1 + pct * (y2 - y1)


def angular_scale_between(
        x1: float, x2: float, x1_value: float, x2_value: float,
        y1: float, y2: float, y1_value: float, y2_value: float,
        x: float, y: float,
        *,
        angular_easing_func=None, gradient_easing_func=None, clamp=True) -> float:
    """
    0 <= x1 < x2 < 1
    0 <= y1 < y2 < 1
    0 <= x < 1
    0 <= y < 1
    "Horizontal" values define the gradient from (x1, 0) to (x2, 0),
    and "vertical" values define the gradient from (0, y1) to (0, y2).
    Everything from (0, 0) to (1, 1) is interpolated to smoothly match
    those.
    """
    angle = math.atan2(y, x)
    return linear_scale_between(
        linear_scale_between(0, math.pi/2, x1, y1, angle, easing_func=angular_easing_func, clamp=clamp),
        linear_scale_between(0, math.pi/2, x2, y2, angle, easing_func=angular_easing_func, clamp=clamp),
        linear_scale_between(0, math.pi/2, x1_value, y1_value, angle, easing_func=angular_easing_func, clamp=clamp),
        linear_scale_between(0, math.pi/2, x2_value, y2_value, angle, easing_func=angular_easing_func, clamp=clamp),
        math.hypot(x, y),
        easing_func=gradient_easing_func, clamp=clamp)


def intensity_at(x: float, y: float) -> float:
    """
    Return the shadow intensity (0.0-1.0) at some point on the canvas.
    24.0 = one tile, but any float position (not just ints) is
    supported, so you can scale to arbitrary resolutions.
    """
    TILE = 24

    TOP_WIDTH = 14
    TOP_INNER_INTENSITY = 0.8

    BOTTOM_WIDTH = 21
    BOTTOM_INNER_INTENSITY = 0.95

    SIDE_WIDTH = 16
    SIDE_INNER_INTENSITY = 0.875

    rel_x = rel_y = 0
    def recompute_relative_pos():
        nonlocal x, y, rel_x, rel_y
        rel_x = x % 24
        rel_y = y % 24

    recompute_relative_pos()

    if 0 <= x < 72 and 0 <= y < 72:  # Main edges and corners
        # Right column is a mirror of the left
        if 48 <= x:
            x = 72 - x
            recompute_relative_pos()

        if 0 <= x < 24:  # Left column
            if 0 <= y < 24:  # Top-left
                return angular_scale_between(
                    0.0, 1.0, SIDE_INNER_INTENSITY, 0.0,
                    0.0, 1.0, TOP_INNER_INTENSITY, 0.0,
                    linear_scale_between(24 - SIDE_WIDTH, 24, 1.0, 0.0, rel_x),
                    linear_scale_between(24 - TOP_WIDTH, 24, 1.0, 0.0, rel_y),
                    angular_easing_func=(lambda x: x**0.4))
                # ^ that easing func fixes an annoying stray pixel in the
                # alpha-channel-bit-depth-reduced version of the tile
            elif 24 <= y < 48:  # Left-middle
                return linear_scale_between(24 - SIDE_WIDTH, 24, 0.0, SIDE_INNER_INTENSITY, rel_x)
            else:  # Bottom-left
                return angular_scale_between(
                    0.0, 1.0, SIDE_INNER_INTENSITY, 0.0,
                    0.0, 1.0, BOTTOM_INNER_INTENSITY, 0.0,
                    linear_scale_between(24 - SIDE_WIDTH, 24, 1.0, 0.0, rel_x),
                    linear_scale_between(0, BOTTOM_WIDTH, 0.0, 1.0, rel_y))
        else:  # Middle column
            if 0 <= y < 24:  # Middle-top
                return linear_scale_between(24 - TOP_WIDTH, 24, 0.0, TOP_INNER_INTENSITY, rel_y)
            elif 24 <= y < 48:  # Middle
                return SIDE_INNER_INTENSITY
            else:  # Middle-bottom
                return linear_scale_between(0, BOTTOM_WIDTH, BOTTOM_INNER_INTENSITY, 0.0, rel_y)

    elif 0 <= x < 48 and 72 <= y < 120:  # Inner corners
        # Left half is a mirror of the right
        if x < 24:
            x = 48 - x
            recompute_relative_pos()

        if 72 <= y < 96:  # top-right (i.e. bottom-right) inner corner
            return clamp(angular_scale_between(
                (24 - SIDE_WIDTH) / 24, 1.0, 0.0, SIDE_INNER_INTENSITY,
                (24 - TOP_WIDTH) / 24, 1.0, 0.0, TOP_INNER_INTENSITY,
                linear_scale_between(0, 24, 0.0, 1.0, rel_x),
                linear_scale_between(0, 24, 0.0, 1.0, rel_y),
                angular_easing_func=ease_in_out_quad,  # has a weird indentation if linear easing is used
                clamp=False))
        else:  # bottom-right (i.e. top-right) inner corner
            return clamp(angular_scale_between(
                (24 - SIDE_WIDTH) / 24, 1.0, 0.0, SIDE_INNER_INTENSITY,
                (24 - BOTTOM_WIDTH) / 24, 1.0, 0.0, BOTTOM_INNER_INTENSITY,
                linear_scale_between(0, 24, 0.0, 1.0, rel_x),
                linear_scale_between(0, 24, 1.0, 0.0, rel_y),
                angular_easing_func=ease_in_out_quad,  # has a weird indentation if linear easing is used
                clamp=False))

    elif 72 <= x < 408:  # Slopes
        if 72 <= x < 96:
            slope_width = 1
            slope_start_x = 72
            slope_direction = 'up'
        elif 96 <= x < 120:
            slope_width = 1
            slope_start_x = 96
            slope_direction = 'down'
        elif 120 <= x < 168:
            slope_width = 2
            slope_start_x = 120
            slope_direction = 'up'
        elif 168 <= x < 216:
            slope_width = 2
            slope_start_x = 168
            slope_direction = 'down'
        elif 216 <= x < 312:
            slope_width = 4
            slope_start_x = 216
            slope_direction = 'up'
        elif 312 <= x < 408:
            slope_width = 4
            slope_start_x = 312
            slope_direction = 'down'

        if 72 <= y < 144:  # Flip directions for ceiling slopes
            slope_direction = 'up' if slope_direction == 'down' else 'down'

        slope_pct = (x - slope_start_x) / (slope_width * 24)

        if slope_direction == 'up':
            delta_y = 24 * (1 - slope_pct)
        else:
            delta_y = 24 * slope_pct

        if 0 <= y < 72:  # Floor
            return clamp(linear_scale_between(
                24 + delta_y - TOP_WIDTH,
                24 + delta_y,
                0.0, TOP_INNER_INTENSITY,
                y, clamp=False))
        elif 72 <= y < 144:  # Ceiling
            return clamp(linear_scale_between(
                24 + delta_y,
                24 + delta_y + BOTTOM_WIDTH,
                BOTTOM_INNER_INTENSITY, 0.0,
                y - 72, clamp=False))

    # Fallback
    return 0.0


def main():
    app = QtGui.QGuiApplication([])

    canvas = QtGui.QImage(408 * SCALE, 144 * SCALE, QtGui.QImage.Format.Format_ARGB32)
    canvas.fill(QtCore.Qt.GlobalColor.transparent)

    # Retail Pa1_gake
    r, g, b = 33, 16, 16
    # AnotherSMBW
    # r = g = b = 0

    for y in range(canvas.height()):
        for x in range(canvas.width()):
            # "+ 0.5" so we sample the *center* of the pixel
            intensity = intensity_at((x + 0.5) / SCALE, (y + 0.5) / SCALE)
            if intensity > 1:
                raise ValueError(f'OOB intensity at {x}, {y}')
            a = int(intensity * 255)
            if a > 0:
                canvas.setPixel(x, y, (a << 24) | (r << 16) | (g << 8) | b)

    canvas.save('Pa1_gake_recreated_shadows.png')


if __name__ == '__main__':
    main()
