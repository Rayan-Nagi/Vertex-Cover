import sys
import re
import operator



class SaveStreet:
    
    def __init__(self,_name=[],_points=[]):
        
        self.name = _name
        self.points = _points


        
def parseInput(line):
    
    if line == '' or line == " " or line == '  ':
        raise Exception ('Wrong Command')
    
    lstCmd = line.strip().split(None, 2)
    
    if line[0] != lstCmd[0]:
        raise Exception ('Wrong input format')
    
    cmd = line[0]
    name = None
    text = None
    points = None
    
    if cmd not in 'arcg':
        raise Exception ('Wrong Command')
        
    if cmd in "ac":
        if len(lstCmd)!=3:
            raise Exception ('Wrong number of arguments for commands (a) or (c)')
        
        else:
            count_qoute = line.count('"')
            if count_qoute !=2:
                raise Exception ('Wrong street name format')
            
            count_brackets1 = line.count('(')
            count_brackets2 = line.count(')')
            
            module_brackets = (count_brackets1 + count_brackets2)%2
            count_comma = line.count(',')
                        
            if count_brackets1<2 or count_brackets2<2 or module_brackets!= 0 or count_brackets1 != count_brackets2 or count_brackets2 != count_comma or count_brackets1 != count_comma:
                raise Exception ('Wrong points input')
            
            text = re.findall(r'"([^"]*)"', line)
            name = str(text[0])
            name = name.lower()
            
            if not all(x.isalpha() or x.isspace() for x in name):
                raise Exception ('Street name should be alphabet only')
                
            coor = re.findall(r'[-+]?\d+', line)
            coor_count=len(coor)
            coor_module= coor_count%2
            
            if coor_count<4 or coor_module!=0:
                raise Exception ('missing coordinates')
            
            points = []
            temp=[int(x) for x in coor]
        
            for i in range(int(len(temp)/2)):
            
                temp2 = [temp[2*i],temp[2*i+1]]
                points.append(tuple(map(int, temp2)))
            
            if len(points)<2:
                raise Exception ('Not enough points for commands (a) or (c)')
                
        
    if cmd == "r":
        if len(lstCmd)<2:
            raise Exception ('Wrong number of arguments for command (r)')
        
        else:
            n = re.findall(r'[-+]?\d+', line)
            for i in range (len(n)):
                if not n[i].isalpha():
                    raise Exception ('Wrong input for command (r)')
            
            count_qoute = line.count('"')
            
            if count_qoute !=2:
                raise Exception ('Wrong street name format')
            
            text = re.findall(r'"([^"]*)"', line)
            name = str(text[0])
            name = name.lower()
            
            if not all(x.isalpha() or x.isspace() for x in name):
                raise Exception ('Street name should be alphabet only')
    
    
    if cmd == 'g':
        if len(lstCmd)!=1:
            raise Exception ('No arguments to be added with command (g)')
    
    return cmd, name, points



def get_segments(streets, street_lines, intersections):

    for s_i in range(0,len(streets)-1):
        for s_j in range(s_i+1,len(streets)):
            s_i_name = streets[s_i].name
            s_j_name = streets[s_j].name
            street_lines[s_i_name] = set()
            street_lines[s_j_name] = set()
            s_i_points = streets[s_i].points
            s_j_points = streets[s_j].points

            for p_i in range(0, len(s_i_points) - 1):
                 for p_j in range(0, len(s_j_points) - 1):
                    l_i = (s_i_points[p_i], s_i_points[p_i + 1])
                    l_j = (s_j_points[p_j], s_j_points[p_j + 1])
                    street_lines[s_i_name].add(l_i)
                    street_lines[s_j_name].add(l_j)
                            
                    inter_point = calculate(l_i, l_j)
                    
                    if inter_point != False:
                        intersections.add(inter_point)
                
    return street_lines, intersections


def is_intersected(intersections, street_lines, temp_list, V_ids, maxid, V, E):

    for intersection in intersections:
        for street_name in street_lines.keys():
            temp_remove_list = []
            temp_add_list = []
            for line in street_lines[street_name]:
                if is_point_on_line(intersection, line):
                    (line_0, line_1) = line
                    add_line_0 = (intersection, line_0)
                    add_line_1 = (intersection, line_1)
                    temp_remove_list.append(line)
                    temp_add_list.append(add_line_0)
                    temp_add_list.append(add_line_1)
    
            for remove_line in temp_remove_list:
                street_lines[street_name].remove(remove_line)
                if remove_line in temp_list:
                    temp_list.remove(remove_line)
                    
            for add_line in temp_add_list:
                street_lines[street_name].add(add_line)
                temp_list.add(add_line)

    nodes = set()
    edges = set()
    for temp_nodes in temp_list:
        edges.add(temp_nodes)
        for node in temp_nodes:
            if node not in V_ids.values():
                maxid += 1
                V_ids[maxid] = node

    for edge in edges:
        (node_0, node_1) = edge
        node_id_0 = 0
        node_id_1 = 0
        for node_id in V_ids.keys():
            if node_0 == V_ids[node_id]:
                node_id_0 = node_id
                V[node_id_0] = node_0
            elif node_1 == V_ids[node_id]:
                node_id_1 = node_id
                V[node_id_1] = node_1
            if node_id_0 and node_id_1:
                break
        E.append((node_id_0, node_id_1))
                
    return street_lines, temp_list, V_ids, maxid, V, E
        

def is_point_on_line(intersection, line):   

    (inter_x, inter_y) = intersection
    (start_x, start_y) = line[0]
    (end_x, end_y) = line[1]
    diffrnce_x_inter = inter_x - start_x
    diffrnce_y_inter = inter_y - start_y
    diffrnce_x_line = end_x - start_x;
    diffrnce_y_line = end_y - start_y;
    X_product = diffrnce_x_inter * diffrnce_y_line - diffrnce_y_inter * diffrnce_x_line
    
    if abs(X_product) < sys.float_info.epsilon * 100:
        if is_between(line, line, inter_x, inter_y):
            return True
    
    return False
    

def calculate(line_i, line_j):

    (B_i, A_i) = tuple(map(operator.sub, line_i[1], line_i[0]))
    (B_j, A_j) = tuple(map(operator.sub, line_j[1], line_j[0]))
    B_i = - B_i
    B_j = - B_j
    C_i = line_i[0][0] * line_i[1][1] - line_i[0][1] * line_i[1][0]
    C_j = line_j[0][0] * line_j[1][1] - line_j[0][1] * line_j[1][0]
    k_y = A_j * B_i - A_i * B_j
    b_y = A_j * C_i - A_i * C_j
    k_x = A_i * B_j - A_j * B_i
    b_x = B_j * C_i - B_i * C_j
    
    if k_y == 0 or k_x == 0:
        return False
    else:
        x = b_x / k_x
        y = b_y / k_y

        if is_between(line_i, line_j, x, y):
            return x, y
        return False
    

def is_between(line_i, line_j, x, y):
    
    max_x_i = max(line_i[0][0], line_i[1][0])
    min_x_i = min(line_i[0][0], line_i[1][0])
    max_y_i = max(line_i[0][1], line_i[1][1])
    min_y_i = min(line_i[0][1], line_i[1][1])
    max_x_j = max(line_j[0][0], line_j[1][0])
    min_x_j = min(line_j[0][0], line_j[1][0])
    max_y_j = max(line_j[0][1], line_j[1][1])
    min_y_j = min(line_j[0][1], line_j[1][1])
    
    if x >= min_x_i and x <= max_x_i and y >= min_y_i and y <= max_y_i:
        if x >= min_x_j and x <= max_x_j and y >= min_y_j and y <= max_y_j:
            return True
        else:
            return False
    else:
        return False


def main ():

    streets = []

    while True:
        try:
            #myline= sys.stdin.readline()
            myline= input()
              
            [cmd, name, points] = parseInput(myline)
        
            if cmd == 'a':
                _newStreet=SaveStreet(name,points)
                
                if len(streets)==0:
                    streets.append(_newStreet)
                
                else:
                    for i in streets:
                        if i.name == _newStreet.name:
                            raise Exception ('Street name already available')
                        
                    streets.append(_newStreet)
                                              
                    
            elif cmd == 'r':
                deleted = False
                _rmStreet = SaveStreet(name)
                
                for i in streets:
                    if i.name == _rmStreet.name:
                        streets.remove(i)
                        deleted=True
                        break
                
                if not deleted:
                    raise Exception ('Street name does not exist')
                
            elif cmd == 'c':
                changed = False
                _chStreet = SaveStreet(name,points)
                
                for i in range(len(streets)):
                    if streets[i].name == _chStreet.name:
                        streets[i] = _chStreet
                        changed=True
                        break
                
                if not changed:
                     raise Exception ('Street name does not exist')
    
            elif cmd == 'g':
                if len(streets)<2:
                    raise Exception ('Not enough streets to calculate intersections')
                
                else:
                    street_lines = {}
                    intersections = set()
                    temp_list = set()
                    V={}
                    E=[]
                    V_ids = {} 
                    maxid = 0
                    
                    street_lines, intersections = get_segments(streets, street_lines, intersections)
                    
                    if len(intersections) > 0:
                        street_lines, temp_list, V_ids, maxid, V, E = is_intersected(intersections,street_lines,temp_list,V_ids,maxid,V,E)
                    
                    
                    print ('V' + ' ' + str(len(V)))
                    sys.stdout.flush()
                    
                    dataid = 0
                    data = "E {"
                    for f in E:
                    	if dataid == len(E) - 1:
                    		if int(f[0]) != 0 and int(f[1]) != 0:
                    			data += '<'+str(int(f[0]-1))+","+str(int(f[1]-1))+'>'
                    	else:
                    		if int(f[0]) != 0 and int(f[1]) != 0:
                    			data += '<'+str(int(f[0]-1))+","+str(int(f[1]-1))+'>,'
                    			dataid += 1
                    
                    data += '}'
                    print(data)
                    sys.stdout.flush()
                 
                    #print('V = {')
                    
                    #for v in V.keys():
                        #print(' '+str(v)+": (" + "%.2f" % V[v][0] + ","+"%.2f"% V[v][1]+ ")")
                    
                    #print('}')
                    #print('E = {')
                    #maxid = 0
                    
                    #for f in E:
                        #if maxid == len(E) - 1:
                            #if int(f[0]) != 0 and int(f[1]) != 0:
                                #print(' <'+str(int(f[0]))+","+str(int(f[1]))+'>')
                        
                        #else:
                            #if int(f[0]) != 0 and int(f[1]) != 0:
                                #print(' <'+str(int(f[0]))+","+str(int(f[1]))+'>,')
                                #maxid += 1
                    
                    #print('}')  
               
        except KeyboardInterrupt:
            print('Error: keyboard interrupt exception', file=sys.stderr)
            sys.exit(0)
        
        except EOFError:
            sys.exit(0)
    
        except Exception as e:
            print('Error: ' + str(e), file=sys.stderr)
    
if __name__ == '__main__':
    main()





