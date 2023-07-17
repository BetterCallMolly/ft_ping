from lib import run_null

def test_valid_ips():
    inputs = [
        "0.0.0.0",
        "255.0.0.0",
        "1.2.3.4",
        "127.0.0.1",
        "44.55.66.77",
        "1.0.0.0",
        "9.9.9.0",
        "9.9.9.9",
    ]
    for arg in inputs:
        _, exit_code = run_null([arg])
        with open("test.txt", "a+") as f:
            f.write(f"{arg}: {exit_code}\n")
        assert exit_code != 0, f"Invalid IP {arg} was accepted. Exit code: {exit_code}"