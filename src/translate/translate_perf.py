import argparse
import cProfile
import translate

if __name__ == "__main__":
    argparser = argparse.ArgumentParser()
    argparser.add_argument(
        "pddl", help="path to domain + task pddl , combined")
    args = argparser.parse_args()
    pddl = str()
    with open(args.pddl) as pddl_file:
        pddl = pddl_file.read()
    problem_start = pddl.find("(define (problem")
    problem = pddl[0:problem_start]
    domain = pddl[problem_start:-1]
    translate.translate_from_strings(problem, domain)
