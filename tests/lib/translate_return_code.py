INVALID_IP = 0b00000001

# NOTE: Before completing the program, the exit code will be a bitmask.

def translate_return_code(return_code: int) -> str:
    if return_code & INVALID_IP:
        return "An invalid IP was passed to the program."
    else:
        return "ok"