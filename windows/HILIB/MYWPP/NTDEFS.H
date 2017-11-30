#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define APIENTRY    FAR PASCAL _export
#define UINT        WORD
#define WPARAM      WORD
#define LPARAM      LONG
