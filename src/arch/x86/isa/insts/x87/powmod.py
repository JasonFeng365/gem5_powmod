microcode = """
def macroop UINT64_POWMOD_R
{
    .adjust_env oszIn64Override
    uint64_powmod
};
def macroop DOUBLE_POW_R
{
    .adjust_env oszIn64Override
    double_pow
};
def macroop UINT64_FIBMOD_R
{
    .adjust_env oszIn64Override
    uint64_fibmod
};

"""