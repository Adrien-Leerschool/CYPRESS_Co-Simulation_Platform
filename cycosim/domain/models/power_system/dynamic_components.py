from typing import List, Union


class DynamicComponent:
    def __init__(self):
        self.id = None
        self.library = None
        self.parameter_file = None
        self.parameter_id = None
        self.static_id = None
        self.static_references = []

    def parse(self, dict_to_parse: dict):
        for key, val in dict_to_parse.items():
            key = key.replace("@", "").replace("dyn:", "")

            if key == "id":
                self.id = val
            elif key == "lib":
                self.library = val
            elif key == "parFile":
                self.parameter_file = val
            elif key == "parId":
                self.parameter_id = val
            elif key == "staticId":
                self.static_id = val
            elif key == "staticRef":
                if isinstance(val, list):
                    for elem in val:
                        stat_ref = StaticReference()
                        stat_ref.parse(elem)
                        self.static_references.append(stat_ref)

                elif isinstance(val, dict):
                    stat_ref = StaticReference()
                    stat_ref.parse(val)
                    self.static_references.append(stat_ref)

                else:
                    print("Error : Unmanaged parsing for class StaticReference.")

            else:
                print(f"Error : Unknown attribute '{key}' for class DynamicComponent.")


class StaticReference:
    def __init__(self):
        self.variable = None
        self.static_variable = None

    def parse(self, dict_to_parse: dict):
        for key, val in dict_to_parse.items():
            key = key.replace("@", "")
            if key == "var":
                self.variable = val
            elif key == "staticVar":
                self.static_variable = val
            else:
                print(f"Error : Unknown attribute '{key}' for class StaticReference")


class Connector:
    def __init__(self):
        self.id = None
        self.connections = []

    def parse(self, dict_to_parse: dict):
        for key, val in dict_to_parse.items():
            key = key.replace("@", "").replace("dyn:", "")

            if key == "id":
                self.id = val
            elif key == "connect":
                if isinstance(val, list):
                    for elem in val:
                        connection = Connection()
                        connection.parse(elem)
                        self.connections.append(connection)

                elif isinstance(val, dict):
                    connection = Connection()
                    connection.parse(val)
                    self.connections.append(connection)

                else:
                    print("Error : Unmanaged parsing for class Connection.")

            else:
                print(f"Error : Unknown attribute '{key}' for class Connector.")


class Connection:
    def __init__(self):
        self.id_1 = None
        self.id_2 = None
        self.variable_1 = None
        self.variable_2 = None
        self.connector = None
        self.index_1 = None

    @property
    def is_macro(self):
        return self.connector is not None

    def parse(self, dict_to_parse: dict):
        for key, val in dict_to_parse.items():
            key = key.replace("@", "")

            if key == "id1":
                self.id_1 = val
            elif key == "id2":
                self.id_2 = val
            elif key == "var1":
                self.variable_1 = val
            elif key == "var2":
                self.variable_2 = val
            elif key == "connector":
                self.connector = val
            elif key == "index1":
                self.index_1 = val
            else:
                print(f"Error : Unknown attribute '{key}' for class Connection.")


class DynamicModel:
    components: List[
        Union[DynamicComponent, StaticReference, Connection, Connector]
    ] = []
