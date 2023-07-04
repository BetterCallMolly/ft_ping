from lib import run_null

def test_invalid_ips():
    inputs = [
        "-0.0.0.0",
        "000.0000.0000.0",
        "127.0.0.256",
        "255.255.255.256",
        "DKSJFHGKHGJSDFKHJGS",
        "0",
        "0.0",
        "0.0.0",
        " ",
        "0"*32,
        "0"*33,
    ]
    for arg in inputs:
        _, exit_code = run_null([arg])
        with open("test.txt", "a+") as f:
            f.write(f"{arg}: {exit_code}\n")
        assert exit_code != 0, f"Invalid IP {arg} was accepted. Exit code: {exit_code}"