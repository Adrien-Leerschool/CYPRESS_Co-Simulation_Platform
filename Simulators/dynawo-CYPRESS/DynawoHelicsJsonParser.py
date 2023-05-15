import json
from os.path import join, expanduser

# Parameters

workingDir = "~/Desktop/dynawo_new/examples/DynaSwing/IEEE39/IEEE39_Cosim"
workingDir = expanduser(workingDir)
parFile = "IEEE39.par"
dynPrefix = "dyn:"

# Main

f = open(join(workingDir, "Dynawo.json"))
dic = json.load(f)
f.close()

pub_index = 0
sub_index = 0
dyd = open(join(workingDir, "dyd.out"), "w")
par = open(join(workingDir, "par.out"), "w")

dyd.write(
    "<"
    + dynPrefix
    + 'blackBoxModel id="CosimInterface" lib="CosimulationAutomaton" parFile="'
    + parFile
    + '" parId="CosimInterface"/>\n'
)

par.write('<set id="CosimInterface">\n')
period = dic["period"]
par.write(
    '\t<par type="DOUBLE" name="automaton_SamplingTime" value="' + str(period) + '"/>\n'
)

if "publications" in dic:
    for pub in dic["publications"]:
        pub_index += 1
        name, model = pub["info"].split("@")
        dyd.write(
            "<"
            + dynPrefix
            + 'connect id1="'
            + model
            + '" var1="'
            + name
            + '" id2="CosimInterface" var2="automaton_inputs_'
            + str(pub_index)
            + '_"/>\n'
        )

        key = pub["key"]
        par.write(
            '\t<par type="STRING" name="automaton_InputsName_'
            + str(pub_index)
            + '_" value="'
            + key
            + '"/>\n'
        )

if "subscriptions" in dic:
    for sub in dic["subscriptions"]:
        sub_index += 1
        name, model = sub["info"].split("@")
        dyd.write(
            "<"
            + dynPrefix
            + 'id1="'
            + model
            + '" var1="'
            + name
            + '"id2="CosimInterface" var2="automaton_outputs_'
            + str(sub_index)
            + '_"/>\n'
        )

        key = sub["key"]
        par.write(
            '\t<par type="STRING" name="automaton_OutputsName_'
            + str(sub_index)
            + '_" value="'
            + key
            + '"/>\n'
        )

par.write(
    '\t<par type="INT" name="automaton_NbInputs" value="' + str(pub_index) + '"/>\n'
)
par.write(
    '\t<par type="INT" name="automaton_NbOutputs" value="' + str(sub_index) + '"/>\n'
)
par.write("</set>\n")

dyd.close()
par.close()
