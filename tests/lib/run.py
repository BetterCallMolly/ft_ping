import subprocess
from typing import List, Tuple
from .translate_return_code import translate_return_code

def run_null(args: List[str]) -> Tuple[str, int]:
    """
    Runs ft_ping with passed arguments (ft_ping argument is optional).
    Returns a tuple of the translated return code and the raw return code.
    """
    if "ft_ping" not in args[0]:
        args.insert(0, "./ft_ping")
    process = subprocess.Popen(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return_code = process.wait()
    return translate_return_code(return_code), return_code