typedef enum _DIVA_DIFFICULTY : uint32_t {
    Easy = 0x0,
    Normal = 0x1,
    Hard = 0x2,
    Extreme = 0x3,
    ExExtreme = 0x4,
} DIVA_DIFFICULTY;
typedef struct _DIVA_PV {
    DIVA_DIFFICULTY Difficulty;
    unsigned int Id;
} DIVA_PV;
typedef struct _DIVA_STAT {
    float CompletionRate;
} DIVA_STAT;
typedef struct _DIVA_SCORE {
    unsigned int TotalScore;
    unsigned int Unknown1;
    unsigned int Unknown2;
    unsigned int Unknown3;
    unsigned int Unknown4;
    unsigned int Unknown5;
    unsigned int Unknown6;
    unsigned int Unknown7;
    unsigned int Combo;
    unsigned int preAdjustCool;
    unsigned int preAdjustFine;
    unsigned int preAdjustSafe;
    unsigned int preAdjustSad;
    unsigned int preAdjustWorst;
    unsigned int Cool;
    unsigned int Fine;
    unsigned int Safe;
    unsigned int Sad;
    unsigned int Worst;
} DIVA_SCORE;