microcode = """
def macroop START_INT32_POWMOD_R
{
    .adjust_env oszIn64Override
    start_int32_powmod
};
def macroop SAVE_INT32_POWMOD_R
{
    .adjust_env oszIn64Override
    save_int32_powmod
};
"""