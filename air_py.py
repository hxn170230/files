class Plane:
    def __init__(self, source, busy_time, id):
        self.source = source
        self.busy_time = busy_time
        self.id = id

    def is_busy(self, time):
        if ( self.busy_time > time ):
            return 1
        else:
            return 0

    def setSource(self, source):
        self.source = source

    def setBusyTime(self, busyTime):
        self.busy_time = busy_time

class Slot:
    def __init__(self, airport):
        self.airport = airport
        self.next_busy_period = [[0 for x in range(2)] for y in range(100)]
        self.next_busy_period_index = -1

    def is_slot_busy(self, land_time):
        # copy from c
        index = 0
        print("Checking slot for land_time:", land_time, airports[self.airport])
        print("next_busy_period_index:", self.next_busy_period_index)
        if (end_time - land_time < 0):
            print("plane cant fly")
            return 1
        if (self.next_busy_period_index == -1):
            return 0
        for index in range (self.next_busy_period_index -1):
            print("current slot:", self.next_busy_period[index][0],
                  self.next_busy_period[index][1], "Next Slot:",
                  self.next_busy_period[index+1][0],
                  self.next_busy_period[index+1][1])
            if (self.next_busy_period[index][0] <= land_time and self.next_busy_period[index][1] >= land_time):
                return 1
            else:
                return 0
        if (index == self.next_busy_period_index-1):
            if (self.next_busy_period[index][0] <= land_time and self.next_busy_period[index][1] >= land_time):
                return 1
        return 0

    def find_and_place(self, cur_time, pl):
        busy_period = [0, 0]
        busy_period[0] = cur_time + flight_time[pl.source][self.airport] - ground_time[self.airport]
        busy_period[1] = cur_time + flight_time[pl.source][self.airport] + ground_time[self.airport]
        i = 0
        index = self.next_busy_period_index
        if (index < 0):
            index = 0
            self.next_busy_period[index][0] = busy_period[0]
            self.next_busy_period[index][1] = busy_period[1]
            self.next_busy_period_index = 1

        for i in range(self.next_busy_period_index-1):
            if ((cur_time + flight_time[pl.source][self.airport] > self.next_busy_period[i][1]) and (busy_period[1] < self.next_busy_period[i+1][0])):
                index = i
        
        if (index <= self.next_busy_period_index-1):
            i = index+1
            temp1 = self.next_busy_period[i+1][0]
            temp2 = self.next_busy_period[i+1][1]
            self.next_busy_period[i+1][0] = self.next_busy_period[i][0]
            self.next_busy_period[i+1][1] = self.next_busy_period[i][1]
            i += 1
            while (i <= self.next_busy_period_index):
                temp3 = self.next_busy_period[i+1][0]
                temp4 = self.next_busy_period[i+1][1]
                self.next_busy_period[i+1][0] = temp1
                self.next_busy_period[i+1][0] = temp2
                temp1 = temp3
                temp2 = temp4
                i+=1

            self.next_busy_period[index+1][0] = busy_period[0]
            self.next_busy_period[index+1][1] = busy_period[1]
            self.next_busy_period_index+=1
        else:
            self.next_busy_period[index][0] = busy_period[0]
            self.next_busy_period[index][1] = busy_period[1]
            self.next_busy_period_index += 1

        print(pl)

    def set_next_busy_period(self, cur_time, pl):
        self.find_and_place(cur_time, pl)
        pl.busy_time = cur_time + flight_time[pl.source][self.airport] + ground_time[self.airport]
        print("Plane:", pl.id, "busy set to", pl.busy_time)


slots = []
slots.append(Slot(0))
slots.append(Slot(0))
slots.append(Slot(1))
slots.append(Slot(2))
slots.append(Slot(2))
slots.append(Slot(2))

min = [[-1, 3, 3], [3, -1, 3], [3, 3, -1]]
start_time = 360
end_time = 1320
airports = ["DAL", "AUS", "HOU"]
ground_time = [30, 25, 35]
flight_time = [[0, 50, 65], [50, 0, 45], [65, 45, 0]]

def getFreeSlots(pl, cur_time):
    # copy from c code
     free_indexes = [-1, -1, -1, -1, -1, -1]
     index = 0
     added = 0
     for index in range (6):
        slot = slots[index]
        if (slot.is_slot_busy(cur_time) == 0):
            free_indexes[index] = slot
            index += 1


     return free_indexes



def getMinIndex(freeList):
    # get min index
    i = 0
    j = 0
    for i in range (6):
        for j in range (3):
            if (freeList[i] == -1):
                continue
            if (min[freeList[i].airport][j] > 0):
                min[freeList[i].airport][j] -= 1
                return i
    return -1

planes = []
planes.append(Plane(2, 0, "T1"))
planes.append(Plane(2, 0, "T2"))
planes.append(Plane(2, 0, "T3"))
planes.append(Plane(0, 0, "T4"))
planes.append(Plane(0, 0, "T5"))
planes.append(Plane(1, 0, "T6"))



for i in range(start_time, end_time):
    pl_index = 0
    
    for pl in planes:
        # check plane busy time with i
        #   if not busy, get free slots
        #       from free slots choose one with min indexes
        #       assign slot to plane
        if (pl.is_busy(i) == 0):
            free_indexes = getFreeSlots(pl, i)
            print(free_indexes)
            x = getMinIndex(free_indexes)
            if (x == -1):
                x = 0
            elif free_indexes[x] != -1:
                slot = free_indexes[x]
                slot.set_next_busy_period(i, pl)
                print("output:", pl.id, airports[pl.source], airports[slot.airport], 
                      i, i+flight_time[pl.source][slot.airport])
                pl.source = slot.airport
