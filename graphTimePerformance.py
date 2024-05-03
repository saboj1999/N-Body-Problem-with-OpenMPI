import matplotlib.pyplot as plt

def readFile(filename):
    _256b_10000 = {}

    with open(filename) as file:
        print("Reading data from "+filename+" : \n")
        for line in file:
            time = float(line.strip().split(" ")[-1].strip())
            steps = int(line.strip().split(" ")[-3].strip())
            bodies = int(line.strip().split(" ")[-4].strip())
            Processes = int(line.strip().split(" ")[-6].strip())
            print(
                 "Time: "+str(time)+"\n"
                +"Body-Count: "+str(bodies)+"\n"
                +"Step-Count: "+str(steps)+"\n"
                +"Processes: "+str(Processes)+"\n"
            )
            _256b_10000[Processes] = time      

    data = {"256 Bodies - 10000 Steps": _256b_10000}     

    return data

def plotEfficiency(data):
    for key, value in data.items():
        plt.plot(list(value.keys()), list(value.values()), color='darkblue')
        plt.scatter(list(value.keys()), list(value.values()), color='green')

        # Loop through each key, value pair in the dictionary
        for x, y in data[key].items():
            # Annotate each point with its x value
            plt.annotate(f'{x}',  # This is the text to be displayed (x value)
                (x, y),  # These are the coordinates to position the text
                textcoords="offset points",  # how to position the text
                xytext=(0,10),  # distance from text to points (x,y)
                ha='center')  # horizontal alignment can be left, right or center

        plt.xscale('log')
        plt.xticks([1, 2, 4, 8, 16])
        plt.xlabel('Processes')
        plt.ylabel('Time (seconds)')
        plt.title(key)
        plt.ylim(12, 20)
        plt.show() 

def GenerateSpeedUpData(data):
    # data is dict of Processes:time
    baseTime = data[1]
    for key, value in data.items():
        data[key] = baseTime / value
    
    return data

def plotSpeedUp(data):
    speedUpData = GenerateSpeedUpData(data["256 Bodies - 10000 Steps"])
    plt.plot(list(speedUpData.keys()), list(speedUpData.values()), color='darkred')
    plt.scatter(list(speedUpData.keys()), list(speedUpData.values()), color='green')

    # Loop through each key, value pair in the dictionary
    for x, y in speedUpData.items():
        # Annotate each point with its x value
        plt.annotate(f'{x}',  # This is the text to be displayed (x value)
                (x, y),  # These are the coordinates to position the text
                textcoords="offset points",  # how to position the text
                xytext=(0,10),  # distance from text to points (x,y)
                ha='center')  # horizontal alignment can be left, right or center

    plt.xscale('log')
    plt.xticks([1, 2, 4, 8, 16])
    plt.xlabel('Processes')
    plt.ylabel('Speed-Up')
    plt.title("256 Bodies - 10000 Steps")
    plt.ylim(0.8, 1.4)
    plt.show() 

def main():
    data = readFile("mpi_average_outputs.txt")
    plotEfficiency(data)
    plotSpeedUp(data)

if __name__ == "__main__":
    main()
