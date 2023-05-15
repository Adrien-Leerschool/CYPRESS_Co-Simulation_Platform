import xmltodict



if __name__ == "__main__" :
    
    path = "/Users/adrienleerschool/Documents/Cypress/cypress-simulator/cypress_simulator/"
    path += "simulators/dynawo_fred/dynawo/examples/DynaSwing/IEEE39/IEEE39_Cosim/IEEE39.iidm"
    
    # Reading the data inside the xml
    # file to a variable under the name
    # data
    with open(path, 'r') as f:
        xml_data = f.read()
     
    my_dict = xmltodict.parse(xml_data)
    
    cnt = 0
    for key, val in my_dict['iidm:network']['iidm:substation'].items() :
        print(f"{key} : {val}")
        if cnt > 4 :
            break 
        cnt += 1
    
    