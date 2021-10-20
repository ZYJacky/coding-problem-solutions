#!/usr/bin/python3
# Homework 1 Code
import numpy as np
import matplotlib.pyplot as plt
import random


def perceptron_learn(data_in):
    # Run PLA on the input data
    #
    # Inputs: data_in: Assumed to be a matrix with each row representing an
    #                (x,y) pair, with the x vector augmented with an
    #                initial 1 (i.e., x_0), and the label (y) in the last column
    # Outputs: w: A weight vector (should linearly separate the data if it is linearly separable)
    #        iterations: The number of iterations the algorithm ran for

    # Your code here, assign the proper values to w and iterations:

    iterations = 0

    w = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0] # Initial zero vector
    while(1):
        sep = True
        for i in range(100):
            x = np.transpose(np.array(data_in[i][0])) # Get the data points
            if int(np.sign(np.dot(x, np.array(w)))) != int(data_in[i][1]): # Misclassified
                sep = False
                for j in range(11):
                    w[j] += int(data_in[i][1]) * data_in[i][0][j] # Adjust weight vector
                break
        iterations+=1
        if(sep):
            break

    return w, iterations


def perceptron_experiment(N, d, num_exp):
    # Code for running the perceptron experiment in HW1
    # Implement the dataset construction and call perceptron_learn; repeat num_exp times
    #
    # Inputs: N is the number of training data points
    #         d is the dimensionality of each data point (before adding x_0)
    #         num_exp is the number of times to repeat the experiment
    # Outputs: num_iters is the # of iterations PLA takes for each experiment
    #          bounds_minus_ni is the difference between the theoretical bound and the actual number of iterations
    # (both the outputs should be num_exp long)

    # Initialize the return variables
    num_iters = np.zeros((num_exp,))
    bounds_minus_ni = np.zeros((num_exp,))

    # Your code here, assign the values to num_iters and bounds_minus_ni:

    for q in range(num_exp):

        data_in = [] # Used to store input data
        rho = 0
        R = 0

        # Generate optimal separator ws
        listws = []
        listws.append(0) # w0 = 0
        for j in range(d):
            listws.append(random.random())
        ws = np.array(listws)

        # Generate 100 data points
        for i in range(N):
            listx = []
            listx.append(1) # x0 = 1
            for k in range(d):
                listx.append(random.uniform(-1, 1))


            x = np.array(listx)
            x = np.transpose(x)
            y = np.sign(np.dot(x, ws)) # Calculate y using optimal separator

            data_in.append([listx, y])

            if(i == 0):     # Keep track of R
                R = np.linalg.norm(x)
            else:
                if(np.linalg.norm(x) > R):
                    R = np.linalg.norm(x)
            
            if(i == 0):     # Keep track of rho
                rho = y * np.dot(x, ws)
            else:
                if((y * np.dot(x, ws)) < rho):
                    rho = y * np.dot(x, ws)
    
        w, iters = perceptron_learn(data_in) # Run learning algorithm

        bound = (R**2) * (((float)(np.linalg.norm(ws)))**2) / (rho**2) # Calculate theoratical upper bound
        bound_minux_ni = bound - iters  # Find difference between theoratical bound and actual iterations
           
        num_iters[q] = iters
        bounds_minus_ni[q] = bound_minux_ni

    return num_iters, bounds_minus_ni


def main():
    print("Running the experiment...")
    num_iters, bounds_minus_ni = perceptron_experiment(100, 10, 1000)

    print("Printing histogram...")
    plt.hist(num_iters)
    plt.title("Histogram of Number of Iterations")
    plt.xlabel("Number of Iterations")
    plt.ylabel("Count")
    plt.show()

    print("Printing second histogram")
    plt.hist(np.log10(bounds_minus_ni))
    plt.title("Bounds Minus Iterations")
    plt.xlabel("Log Difference of Theoretical Bounds and Actual # Iterations")
    plt.ylabel("Count")
    plt.show()

if __name__ == "__main__":
    main()