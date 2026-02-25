microcode = """
def macroop INT32_POWMOD_R
{
    .adjust_env oszIn64Override
    int32_powmod
};
"""