import json


class HelicsSerializerJSON:
    def __init__(self, _output_path: str, _cosim_obj):
        self.output_path = _output_path
        self.cosim_obj = _cosim_obj

    def serialize(self) -> None:
        out_dict = dict()

        out_dict["name"] = self.cosim_obj.name
        out_dict["core_type"] = self.cosim_obj.core_type
        out_dict["terminate_on_error"] = self.cosim_obj.terminate_on_error
        out_dict["source_only"] = self.cosim_obj.source_only
        out_dict["observer"] = self.cosim_obj.observer
        out_dict["only_update_on_change"] = self.cosim_obj.only_update_on_change
        out_dict["log_level"] = self.cosim_obj.log_level
        out_dict["uninterruptible"] = self.cosim_obj.uninterruptible
        out_dict["period"] = self.cosim_obj.period

        if self.cosim_obj.publications:
            out_dict["publications"] = [pub.to_dict() for pub in self.cosim_obj.publications]
        if self.cosim_obj.subscriptions:
            out_dict["subscriptions"] = [sub.to_dict() for sub in self.cosim_obj.subscriptions]

        with open(self.output_path, "w", encoding="utf-8") as out_file:
            json.dump(out_dict, out_file, indent=2)
