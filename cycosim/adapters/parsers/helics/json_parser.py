import json  # noqa

from cycosim.domain.ports.parser import Parser


class HelicsParserJSON(Parser):
    json_dict = dict()
