def remove_superfluous(xml_dict: dict, to_remove: list[str]):
    """_summary_
    Return the given dictionary where the key values have been freed from the
    elements of the 'to_remove' list.

    Args:
        xml_dict (dict): The dictionary to clean
        to_remove (list): The elements to remove from the keys

    Returns:
        light_dict (dict): The cleaned dictionary
    """
    light_dict = {}
    for key, val in xml_dict.items():
        if isinstance(val, dict):
            for t_r in to_remove:
                key = key.replace(t_r, "")
            light_dict[key] = remove_superfluous(val, to_remove)

        elif isinstance(val, list):
            for t_r in to_remove:
                key = key.replace(t_r, "")
            light_dict[key] = [remove_superfluous(elem, to_remove) for elem in val]

        else:
            for t_r in to_remove:
                key = key.replace(t_r, "")

            light_dict[key] = val
    return light_dict
