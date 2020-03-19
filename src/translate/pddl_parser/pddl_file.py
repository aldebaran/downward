import options

from . import lisp_parser
from . import parsing_functions

def parse_pddl_file(type, filename):
    try:
        # The builtin open function is shadowed by this module's open function.
        # We use the Latin-1 encoding (which allows a superset of ASCII, of the
        # Latin-* encodings and of UTF-8) to allow special characters in
        # comments. In all other parts, we later validate that only ASCII is
        # used.
        return lisp_parser.parse_nested_list(file_open(filename,
                                                       encoding='ISO-8859-1'))
    except OSError as e:
        raise SystemExit("Error: Could not read file: %s\nReason: %s." %
                         (e.filename, e))
    except lisp_parser.ParseError as e:
        raise SystemExit("Error: Could not parse %s file: %s\nReason: %s." %
                         (type, filename, e))

def parse_lisp_stream(type, stream, filename =""):
    """Parses the PDDL content from a stream, that can be read line by line."""
    try:
        return lisp_parser.parse_nested_list(stream)
    except lisp_parser.ParseError as e:
        raise SystemExit("Error: Could not parse %s file: %s\nReason: %s." %
                         (type, filename, e))

def parse_pddl_streams(domain, task, domain_filename = "", task_filename = ""):
    """Parse PDDL domain and problem from streams that can be read line by line."""
    domain_lisp = parse_lisp_stream("domain", domain, domain_filename)
    task_lisp = parse_lisp_stream("task", task, task_filename)
    return parsing_functions.parse_task(domain_lisp, task_lisp)

def open(domain_filename=None, task_filename=None):
    domain_filename = domain_filename or options.domain
    task_filename = task_filename or options.task

    # The builtin open function is shadowed by this module's open function.
    # We use the Latin-1 encoding (which allows a superset of ASCII, of the
    # Latin-* encodings and of UTF-8) to allow special characters in
    # comments. In all other parts, we later validate that only ASCII is
    # used.
    domain_stream = None
    task_stream = None
    try:
        domain_stream = file_open(domain_filename, encoding='ISO-8859-1')
        task_stream = file_open(task_filename, encoding='ISO-8859-1')
    except IOError as e:
        raise SystemExit("Error: Could not read file: %s\nReason: %s." %
                         (e.filename, e))
    return parse_pddl_streams(domain_stream, task_stream, domain_filename, task_filename)
