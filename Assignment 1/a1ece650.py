import re


class Point(object):

    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)


class Line(object):
    def __init__(self, src, dst):
        self.src = src
        self.dst = dst


def onSegment(x1, x2, xcoor, y1, y2, ycoor):
    return min(x1, x2) <= xcoor and xcoor <= max(x1, x2) and min(y1, y2) <= ycoor and ycoor <= max(y1, y2)


def checkIntersect(line1, line2):
    x1 = line1.src.x
    y1 = line1.src.y

    x2 = line1.dst.x
    y2 = line1.dst.y

    x3 = line2.src.x
    y3 = line2.src.y

    x4 = line2.dst.x
    y4 = line2.dst.y

    xnum = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4))
    xnum = 0.0 if xnum == -0.0 else xnum

    ynum = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)
    ynum = 0.0 if ynum == -0.0 else ynum

    den = ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4))

    if den != 0:
        xcoor = xnum / den
        xcoor = 0.0 if xcoor == -0.0 else xcoor

        ycoor = ynum / den
        ycoor = 0.0 if ycoor == -0.0 else ycoor

        if onSegment(x1, x2, xcoor, y1, y2, ycoor) and onSegment(x3, x4, xcoor, y3, y4, ycoor):
            return (xcoor, ycoor)

    if den == 0:
        xden_1 = (x3 - x1) * (y2 - y4) - (x2 - x4) * (y3 - y1)
        xden_2 = (x4 - x1) * (y3 - y2) - (x3 - x2) * (y4 - y1)

        if xden_1 == 0 and xden_2 == 0:
            return 'parallel'

    return None


def isValidNumberinParentheses(points):
    for segment in re.findall("[(][^)]*[)]", points):
        if len(re.findall("[-+]?\d+[\.]?\d*[eE]?[-+]?\d*", segment)) != 2:
            return False
    return True


def checkParentheses(points):
    c = 0
    if "(" not in points:
        return False

    if ")" not in points:
        return False

    for char in points:
        if char == "(":
            c += 1
        elif char == ")":
            c -= 1
            if c < 0:
                return False

    if c == 0:
        return True
    return False


def checkStreetName(street):
    if street == "":
        return False
    for x in street:
        if ((not x.isalpha()) and (not x.isspace())):
            return False
    return True


def getPoints(s):
    c = []
    for i in s:
        if i.isdigit():
            c.append(float(i))
    points = []
    for i in range(0, len(c), 2):
        points.append((c[i], c[i + 1]))

    return points

def generateVertexEdges():
    intersection_line = []
    mapping_temp = mapping
    all_points = list(mapping_temp.values())

    for i in range(len(all_points)):
        for j in range(i + 1, len(all_points)):
            for m in range(len(all_points[i]) - 1):
                point1 = Point(all_points[i][m][0], all_points[i][m][1])
                point2 = Point(all_points[i][m + 1][0], all_points[i][m + 1][1])
                line1 = Line(point1, point2)

                for n in range(len(all_points[j]) - 1):
                    point3 = Point(all_points[j][n][0], all_points[j][n][1])
                    point4 = Point(all_points[j][n + 1][0], all_points[j][n + 1][1])
                    line2 = Line(point3, point4)

                    result = checkIntersect(line1, line2)

                    if result and result != 'parallel':
                        if result not in vertex_id:
                            vertex_id.append(result)

                        if [all_points[j][n], result, all_points[j][n + 1]] not in intersection_line:
                            intersection_line.append([all_points[j][n], result, all_points[j][n + 1]])

                        if [all_points[i][m], result, all_points[i][m + 1]] not in intersection_line:
                            intersection_line.append([all_points[i][m], result, all_points[i][m + 1]])

                    elif result == 'parallel':
                        line3 = Line(point1, point3)
                        line4 = Line(point2, point4)
                        new_result = checkIntersect(line3, line4)

                        if new_result == "parallel":
                            four_points = [all_points[i][m], all_points[i][m + 1], all_points[j][n],
                                           all_points[j][n + 1]]
                            four_points = sorted(four_points)

                            intersection_line.append([four_points[0], four_points[1], four_points[2], four_points[3]])

    new_intersection_line = []
    temp = []

    for j in range(len(intersection_line) - 1):
        for i in range(j + 1, len(intersection_line)):

            if intersection_line[j][0] == intersection_line[i][0] and intersection_line[j][-1] == intersection_line[i][
                -1]:
                for p in intersection_line[j]:
                    temp.append(p)
                for q in intersection_line[i]:
                    if q not in temp:
                        temp.append(q)

                new_intersection_line.append(sorted(temp))

                del intersection_line[i]
                del intersection_line[j]

                temp = []
                break

        if j == len(intersection_line) - 2:
            break

    vertices = []

    for line in new_intersection_line:
        intersection_line.append(line)

    for line in intersection_line:
        for point in line:
            if point not in vertices:
                vertices.append(point)

    final_vertices = {}
    final_edges = []

    for vertex in vertices:
        index = vertex_id.index(vertex)
        final_vertices[index] = vertex

    for i in range(len(intersection_line)):
        for j in range(len(intersection_line[i]) - 1):

            first = list(final_vertices.keys())[list(final_vertices.values()).index(intersection_line[i][j])]
            second = list(final_vertices.keys())[list(final_vertices.values()).index(intersection_line[i][j + 1])]

            if first != second:
                res = (first, second) if first < second else (second, first)
                final_edges.append(res)

    print ("V = {")
    for index in final_vertices:
        first = str(int(round(final_vertices[index][0], 2)))
        second = str(int(round(final_vertices[index][1], 2)))
        print (' ' + str(index) + ': (' + first + ',' + second + ')')
    print ("}")

    final_edges = list(set(final_edges))

    print ("E = {")
    for i in range(len(final_edges)):
        edge = ' <' + str(final_edges[i][0]) + ',' + str(final_edges[i][1]) + '>'
        if i != len(final_edges) - 1:
            edge += ','
        print (edge)
    print ('}')


mapping = {}
vertex_id = []


def main():
    while True:

        # print "Please input your command: ",
        Input = input()

        if Input == '':
            break

        if 'g' == Input.strip(' '):
            generateVertexEdges()

        elif len(Input.split('"')) == 3:

            input_list = Input.split('"')

            if checkParentheses(input_list[2]) and isValidNumberinParentheses(input_list[2]) and checkStreetName(
                    input_list[1]) and input_list[2].split(' ')[0] == "":

                points = getPoints(input_list[2])

                if 'a' == input_list[0].strip(' ') and 'a ' in input_list[0]:
                    if input_list[1].lower() in mapping:
                        print ("Error: street currently exists.")
                    else:
                        mapping[input_list[1].lower()] = points

                elif 'c' in input_list[0].strip(' ') and 'c ' in input_list[0]:
                    if input_list[1].lower() not in mapping:
                        print ("Error: 'c' or 'r' specified for a street that does not exist.")
                    else:
                        mapping[input_list[1].lower()] = points

                else:
                    print ("Error: Command is not valid!")

                for i in points:
                    if i not in vertex_id:
                        vertex_id.append(i)

            elif 'r' in input_list[0].strip(' ') and checkStreetName(input_list[1]) and 'r ' in input_list[0]:
                if input_list[1].lower() not in mapping:
                    print ("Error: Incorrect input format")
                else:
                    del mapping[input_list[1].lower()]
            else:
                print ("Error: Incorrect input format")

        else:
            print ("Error: Incorrect input format")


if __name__ == '__main__':
    main()