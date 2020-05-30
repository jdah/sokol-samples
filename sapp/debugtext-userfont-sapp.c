//------------------------------------------------------------------------------
//  debugtext-userfont-sapp.c
//  sokol_debugtext.h: render with user-provided font data (Atari 400 ROM extract)
//------------------------------------------------------------------------------
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#define SOKOL_DEBUGTEXT_IMPL
#include "sokol_debugtext.h"
#include "dbgui/dbgui.h"

typedef struct {
    uint8_t r, g, b;
} rgb_t;

static struct {
    sg_pass_action pass_action;
    uint32_t frame_count;
    rgb_t color_palette[16];
} state = {
    .pass_action = {
        .colors[0] = { .action = SG_ACTION_CLEAR, .val = { 0.0f, 0.125f, 0.25f, 1.0f } }
    },
    .color_palette = {
        { 0xf4, 0x43, 0x36 },
        { 0xe9, 0x1e, 0x63 },
        { 0x9c, 0x27, 0xb0 },
        { 0x67, 0x3a, 0xb7 },
        { 0x3f, 0x51, 0xb5 },
        { 0x21, 0x96, 0xf3 },
        { 0x03, 0xa9, 0xf4 },
        { 0x00, 0xbc, 0xd4 },
        { 0x00, 0x96, 0x88 },
        { 0x4c, 0xaf, 0x50 },
        { 0x8b, 0xc3, 0x4a },
        { 0xcd, 0xdc, 0x39 },
        { 0xff, 0xeb, 0x3b },
        { 0xff, 0xc1, 0x07 },
        { 0xff, 0x98, 0x00 },
        { 0xff, 0x57, 0x22 }
    }
};

// use font slot 1 for our user font (can be anything between 0 and SDTX_MAX_FONTS)
#define USER_FONT (1)

// forward declared font data, the actual font data is at the end of this source file
static const uint8_t user_font[128 * 8];

static void init(void) {
    // setup sokol-gfx
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext()
    });
    __dbgui_setup(sapp_sample_count());

    /*  setup sokol-debugtext with the user font as the only font,
        NOTE that the user font only provides pixel data for the
        characters 0x20 to 0x9F inclusive
    */
    sdtx_setup(&(sdtx_desc_t){
        .fonts[USER_FONT] = {
            .ptr = user_font,
            .size = sizeof(user_font),
            .first_char = 0x20,
            .last_char  = 0x9F
        }
    });
}

static void frame(void) {
    state.frame_count++;

    sdtx_canvas(sapp_width() * 0.25f, sapp_height() * 0.25f);
    sdtx_origin(1, 2);
    sdtx_font(USER_FONT);
    sdtx_color3b(0xff, 0x17, 0x44);
    sdtx_puts("Hello 8-bit ATARI font:\n\n");
    uint32_t line = 0;
    for (int c = 0x20; c < 0xA0; c++) {
        if ((c & 15) == 0) {
            sdtx_puts("\n\t");
            line++;
        }
        // color scrolling effect:
        const rgb_t rgb = state.color_palette[(c + line + (state.frame_count / 2)) & 15];
        sdtx_color3b(rgb.r, rgb.g, rgb.b);
        sdtx_putc(c);
    }

    sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
    sdtx_draw();
    __dbgui_draw();
    sg_end_pass();
    sg_commit();
}

static void cleanup(void) {
    sdtx_shutdown();
    __dbgui_shutdown();
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return (sapp_desc) {
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = __dbgui_event,
        .width = 800,
        .height = 600,
        .gl_force_gles2 = true,
        .window_title = "debugtext-userfont-sapp",
    };
}

// Font data extracted from Atari 400 ROM at address 0xE000,
// and reshuffled to map to ASCII. Each character is 8 bytes,
// 1 bit per pixel in an 8x8 matrix.
static const uint8_t user_font[128 * 8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 20
    0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00, // 21
    0x00, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, // 22
    0x00, 0x66, 0xFF, 0x66, 0x66, 0xFF, 0x66, 0x00, // 23
    0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00, // 24
    0x00, 0x66, 0x6C, 0x18, 0x30, 0x66, 0x46, 0x00, // 25
    0x1C, 0x36, 0x1C, 0x38, 0x6F, 0x66, 0x3B, 0x00, // 26
    0x00, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, // 27
    0x00, 0x0E, 0x1C, 0x18, 0x18, 0x1C, 0x0E, 0x00, // 28
    0x00, 0x70, 0x38, 0x18, 0x18, 0x38, 0x70, 0x00, // 29
    0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00, // 2A
    0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00, // 2B
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, // 2C
    0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, // 2D
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, // 2E
    0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00, // 2F
    0x00, 0x3C, 0x66, 0x6E, 0x76, 0x66, 0x3C, 0x00, // 30
    0x00, 0x18, 0x38, 0x18, 0x18, 0x18, 0x7E, 0x00, // 31
    0x00, 0x3C, 0x66, 0x0C, 0x18, 0x30, 0x7E, 0x00, // 32
    0x00, 0x7E, 0x0C, 0x18, 0x0C, 0x66, 0x3C, 0x00, // 33
    0x00, 0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x00, // 34
    0x00, 0x7E, 0x60, 0x7C, 0x06, 0x66, 0x3C, 0x00, // 35
    0x00, 0x3C, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00, // 36
    0x00, 0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x00, // 37
    0x00, 0x3C, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00, // 38
    0x00, 0x3C, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00, // 39
    0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, // 3A
    0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30, // 3B
    0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00, // 3C
    0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, // 3D
    0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00, // 3E
    0x00, 0x3C, 0x66, 0x0C, 0x18, 0x00, 0x18, 0x00, // 3F
    0x00, 0x3C, 0x66, 0x6E, 0x6E, 0x60, 0x3E, 0x00, // 40
    0x00, 0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x00, // 41
    0x00, 0x7C, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00, // 42
    0x00, 0x3C, 0x66, 0x60, 0x60, 0x66, 0x3C, 0x00, // 43
    0x00, 0x78, 0x6C, 0x66, 0x66, 0x6C, 0x78, 0x00, // 44
    0x00, 0x7E, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00, // 45
    0x00, 0x7E, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00, // 46
    0x00, 0x3E, 0x60, 0x60, 0x6E, 0x66, 0x3E, 0x00, // 47
    0x00, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, // 48
    0x00, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, // 49
    0x00, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3C, 0x00, // 4A
    0x00, 0x66, 0x6C, 0x78, 0x78, 0x6C, 0x66, 0x00, // 4B
    0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00, // 4C
    0x00, 0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x00, // 4D
    0x00, 0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x00, // 4E
    0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, // 4F
    0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x00, // 50
    0x00, 0x3C, 0x66, 0x66, 0x66, 0x6C, 0x36, 0x00, // 51
    0x00, 0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x00, // 52
    0x00, 0x3C, 0x60, 0x3C, 0x06, 0x06, 0x3C, 0x00, // 53
    0x00, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, // 54
    0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7E, 0x00, // 55
    0x00, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, // 56
    0x00, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00, // 57
    0x00, 0x66, 0x66, 0x3C, 0x3C, 0x66, 0x66, 0x00, // 58
    0x00, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00, // 59
    0x00, 0x7E, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00, // 5A
    0x00, 0x1E, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00, // 5B
    0x00, 0x40, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00, // 5C
    0x00, 0x78, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00, // 5D
    0x00, 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, // 5E
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, // 5F
    0x00, 0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18, 0x00, // 60
    0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00, // 61
    0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x7C, 0x00, // 62
    0x00, 0x00, 0x3C, 0x60, 0x60, 0x60, 0x3C, 0x00, // 63
    0x00, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3E, 0x00, // 64
    0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00, // 65
    0x00, 0x0E, 0x18, 0x3E, 0x18, 0x18, 0x18, 0x00, // 66
    0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x7C, // 67
    0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x00, // 68
    0x00, 0x18, 0x00, 0x38, 0x18, 0x18, 0x3C, 0x00, // 69
    0x00, 0x06, 0x00, 0x06, 0x06, 0x06, 0x06, 0x3C, // 6A
    0x00, 0x60, 0x60, 0x6C, 0x78, 0x6C, 0x66, 0x00, // 6B
    0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, // 6C
    0x00, 0x00, 0x66, 0x7F, 0x7F, 0x6B, 0x63, 0x00, // 6D
    0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00, // 6E
    0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00, // 6F
    0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, // 70
    0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x06, // 71
    0x00, 0x00, 0x7C, 0x66, 0x60, 0x60, 0x60, 0x00, // 72
    0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00, // 73
    0x00, 0x18, 0x7E, 0x18, 0x18, 0x18, 0x0E, 0x00, // 74
    0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00, // 75
    0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, // 76
    0x00, 0x00, 0x63, 0x6B, 0x7F, 0x3E, 0x36, 0x00, // 77
    0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x00, // 78
    0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x0C, 0x78, // 79
    0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00, // 7A
    0x00, 0x18, 0x3C, 0x7E, 0x7E, 0x18, 0x3C, 0x00, // 7B
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, // 7C
    0x00, 0x7E, 0x78, 0x7C, 0x6E, 0x66, 0x06, 0x00, // 7D
    0x08, 0x18, 0x38, 0x78, 0x38, 0x18, 0x08, 0x00, // 7E
    0x10, 0x18, 0x1C, 0x1E, 0x1C, 0x18, 0x10, 0x00, // 7F
    0x00, 0x36, 0x7F, 0x7F, 0x3E, 0x1C, 0x08, 0x00, // 80
    0x18, 0x18, 0x18, 0x1F, 0x1F, 0x18, 0x18, 0x18, // 81
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, // 82
    0x18, 0x18, 0x18, 0xF8, 0xF8, 0x00, 0x00, 0x00, // 83
    0x18, 0x18, 0x18, 0xF8, 0xF8, 0x18, 0x18, 0x18, // 84
    0x00, 0x00, 0x00, 0xF8, 0xF8, 0x18, 0x18, 0x18, // 85
    0x03, 0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xC0, // 86
    0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x03, // 87
    0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, // 88
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, // 89
    0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, // 8A
    0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, // 8B
    0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, // 8C
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 8D
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, // 8E
    0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, // 8F
    0x00, 0x1C, 0x1C, 0x77, 0x77, 0x08, 0x1C, 0x00, // 90
    0x00, 0x00, 0x00, 0x1F, 0x1F, 0x18, 0x18, 0x18, // 91
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, // 92
    0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18, // 93
    0x00, 0x00, 0x3C, 0x7E, 0x7E, 0x7E, 0x3C, 0x00, // 94
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, // 95
    0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, // 96
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0x18, 0x18, 0x18, // 97
    0x18, 0x18, 0x18, 0xFF, 0xFF, 0x00, 0x00, 0x00, // 98
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, // 99
    0x18, 0x18, 0x18, 0x1F, 0x1F, 0x00, 0x00, 0x00, // 9A
    0x78, 0x60, 0x78, 0x60, 0x7E, 0x18, 0x1E, 0x00, // 9B
    0x00, 0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x00, // 9C
    0x00, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00, // 9D
    0x00, 0x18, 0x30, 0x7E, 0x30, 0x18, 0x00, 0x00, // 9E
    0x00, 0x18, 0x0C, 0x7E, 0x0C, 0x18, 0x00, 0x00, // 9F
};
