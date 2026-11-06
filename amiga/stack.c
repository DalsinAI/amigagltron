/* A Shell gives 4 KB of stack; libnix's startup sets this much up instead.
 * The AmiCradle Team. GPL 2 or later, like GLtron. */
unsigned long __stack = 512 * 1024;
static const char version[] __attribute__((used)) = "$VER: GLtron 0.70 Amiga (06.11.2026)";
