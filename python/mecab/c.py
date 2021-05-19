import os
import platform


def _is_mac():
    return platform.system() == "Darwin"


def get_static_lib_name():
    return ["libmecab.a"] if _is_mac() else []


def get_shared_lib_name():
    return ["libmecab.so.2"] if _is_mac() else []


def get_lib_name():
    return get_static_lib_name() + get_shared_lib_name()


def get_include_dir():
    return [os.path.join(os.path.dirname(__file__), "include")]
