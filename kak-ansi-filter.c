#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

#define DEFAULT 9

typedef struct {
    int line;
    int column;
} Coord;

typedef struct {
    int foreground;
} Attributes;

static const wchar_t* COLORS[] =
{
    L"black",
    L"red",
    L"green",
    L"yellow",
    L"blue",
    L"magenta",
    L"cyan",
    L"white",
    L"-",
    L"default",
};

static const Attributes NO_ATTRIBUTES =
{
    .foreground = DEFAULT
};

Coord      current_coord          = { .line = 1, .column = 1 };
Coord      previous_char_coord    = { .line = 1, .column = 0 };
Coord      color_start_coord      = { .line = 1, .column = 1 };
Attributes current_attributes     = { .foreground = DEFAULT };
wchar_t    escape_sequence[1024];
int        escape_sequence_length = 0;

bool attributes_equal(const Attributes* a, const Attributes* b)
{
    return a->foreground == b->foreground;
}

int parse_codes(const wchar_t* p, int* codes, int max_codes)
{
    int count = 0;
    for (; *p && count < max_codes; p++)
    {
        if (!iswdigit(p[0]) && iswdigit(p[1]))
        {
            codes[count] = -1;
            swscanf(p + 1, L"%d", &codes[count]);
            if (codes[count] == -1)
                continue;
            ++count;
        }
    }
    return count;
}

void process_ansi_escape(wchar_t* seq)
{
    int codes[512];
    Attributes previous_attributes = current_attributes;
    int code_count = parse_codes(seq, codes, sizeof(codes)/sizeof(codes[0]));

    for (int i = 0; i < code_count; i++)
    {
        int code = codes[i];
        if (code >= 30 && code <= 39)
        {
            current_attributes.foreground = code % 10;
        }
    }

    if (!attributes_equal(&previous_attributes, &current_attributes) &&
        !attributes_equal(&previous_attributes, &NO_ATTRIBUTES))
    {
        fwprintf(stderr, L" %d.%d,%d.%d|%ls",
                 color_start_coord.line, color_start_coord.column,
                 previous_char_coord.line, previous_char_coord.column,
                 COLORS[previous_attributes.foreground]);
        color_start_coord = current_coord;
    }
}

void process_escape_sequence(wchar_t* seq)
{
    if (wcslen(seq) < 2)
        return;
    if (seq[1] == L'[' && seq[wcslen(seq)-1] == 'm')
        process_ansi_escape(seq);
}

void add_escape_char(wchar_t ch)
{
    if (escape_sequence_length >= 1020)
        return;
    escape_sequence[escape_sequence_length++] = ch;
    escape_sequence[escape_sequence_length] = L'\0';
}

bool handle_escape_char(wchar_t ch)
{
    if (ch == 0x1b && escape_sequence_length == 0)
    {
        add_escape_char(ch);
        return true;
    }
    if (ch == L'[' && escape_sequence_length == 1)
    {
        add_escape_char(ch);
        return true;
    }
    if ((ch == L';' || iswdigit(ch)) && escape_sequence_length > 1)
    {
        add_escape_char(ch);
        return true;
    }
    if (escape_sequence_length > 1)
    {
        add_escape_char(ch);
        process_escape_sequence(escape_sequence);
        escape_sequence_length = 0;
        return true;
    }
    return false;
}

void display_char(wchar_t ch)
{
    putwchar(ch);
    previous_char_coord = current_coord;
    if (ch == L'\n')
    {
        ++current_coord.line;
        current_coord.column = 1;
    }
    else
        ++current_coord.column;
}

int main(int argc, char* argv[])
{
    wchar_t ch;

    setlocale(LC_ALL, "en_US.utf8");

    fwprintf(stderr, L"set-option -add buffer ansi_color_ranges");

    while ((ch = fgetwc(stdin)) != WEOF)
    {
        if (handle_escape_char(ch))
            continue;
        display_char(ch);
    }

    fwprintf(stderr, L"\n");
    exit(0);
}