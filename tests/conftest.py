

def pytest_addoption(parser):
    parser.addoption(
        "--bin",
        action="store",
        default="",
        help="Path to compiled binary for test"
    )