#!/usr/bin/python3
# Homework 2 Code
import numpy as np
import pandas as pd
from math import *
import time


def find_binary_error(w, X, y):
    # find_binary_error: compute the binary error of a linear classifier w on data set (X, y)
    # Inputs:
    #        w: weight vector
    #        X: data matrix (without an initial column of 1s)
    #        y: data labels (plus or minus 1)
    # Outputs:
    #        binary_error: binary classification error of w on the data set (X, y)
    #           this should be between 0 and 1.

    # Your code here, assign the proper value to binary_error:

    binary_error = 0
    N = 0
    for x in X:
        x = np.insert(x, 0, [1]) # Append initial column of 1
        s = np.dot(np.transpose(w),x)
        theta = 1/(1 + e**(-s))  # Sigmoid function

        if theta >= 0.5:
            prediction = 1
        else:
            prediction = -1
        
        binary_error += (prediction != y[N])
        N+=1

    binary_error = binary_error/N
    
    return binary_error


def logistic_reg(X, y, w_init, max_its, eta, grad_threshold):
    # logistic_reg learn logistic regression model using gradient descent
    # Inputs:
    #        X : data matrix (without an initial column of 1s)
    #        y : data labels (plus or minus 1)
    #        w_init: initial value of the w vector (d+1 dimensional)
    #        max_its: maximum number of iterations to run for
    #        eta: learning rate
    #        grad_threshold: one of the terminate conditions; 
    #               terminate if the magnitude of every element of gradient is smaller than grad_threshold
    # Outputs:
    #        t : number of iterations gradient descent ran for
    #        w : weight vector
    #        e_in : in-sample error (the cross-entropy error as defined in LFD)

    # Your code here, assign the proper values to t, w, and e_in:

    t = 0
    w = w_init
    e_in = 0
    X_t = [] 

    for x in X:
        X_t.append(np.insert(x, 0, [1])) # Append initial column of 1
    X_t = np.array(X_t)
    
    while t < max_its:

        gradient = 0
        N = 0
        sum = 0

        numerators = y * X_t
        wX = X_t.dot(np.transpose(w))
        wX = np.split(wX,len(wX))  #len(wX) is the # of data points
        wX = np.array(wX)
        ywX = y*wX 

        denominators = 1 + e**(ywX)
        sum = numerators/denominators
        sum = np.sum(sum, axis=0)
        gradient = -sum/len(wX)
        
        # Break if every element of gradient is less than threshold
        if (all((abs(gradient) < grad_threshold))):
            break

        w = w - (eta*gradient)

        t+=1

    n = 0   
    wX = X_t.dot(np.transpose(w))

    # Calculate in sample error        
    for x in X_t:    
        e_in += np.log(1 + e**(-y[n] * wX[n]))
        n+=1

    e_in = e_in/n
        
    
    return t, w, e_in


def main():
    # Load training data
    train_data_x = pd.read_csv('clevelandtrain.csv', usecols=[i for i in range(13)])
    train_X = np.array(train_data_x)

    # Load and process training label
    train_label_y = pd.read_csv('clevelandtrain.csv', usecols=["heartdisease::category|0|1"])
    train_y = np.array(train_label_y)
    for label in train_y:  # Change label from 0/1 to -1/1
        if label[0] == 0:
            label[0] = -1

    # Load test data
    test_data_x = pd.read_csv('clevelandtest.csv', usecols=[i for i in range(13)])
    test_X = np.array(test_data_x)

    # Load and process testing label
    test_label_y = pd.read_csv('clevelandtest.csv', usecols=["heartdisease::category|0|1"])
    test_y = np.array(test_label_y)
    for label in test_y:  # Change label from 0/1 to -1/1
        if label[0] == 0:
            label[0] = -1

    print(test_y.shape)
    # Your code here

    # Initialize training parameters
    w_init = np.zeros(14)
    max_its = 1e6
    eta = 1e-5
    grad_threshold = 1e-3
    
    for max_its in [1e4, 1e5, 1e6]:
        # Run logistic regression on train data
        train_start = time.time()
        t, w, e_in = logistic_reg(train_X, train_y, w_init, max_its, eta, grad_threshold)
        train_end = time.time()
    
        # Find binary error on both test and train data
    
        test_binary_error = find_binary_error(w, test_X, test_y)
        train_binary_error = find_binary_error(w, train_X, train_y)

        print("Traing time: " + str(train_end-train_start))
        print("Traing iterations: " + str(t))
        print("In-sample error:" + str(e_in))
        print("Train binary error: " + str(train_binary_error))
        print("Test binary_error: " + str(test_binary_error))
        print("\n") # Make 2 empty lines
    
    
    # Normalize training and test set using the distribution parameters of training set
    for i in range(train_X.shape[1]):
        feature = train_X[:,i]
        mean = np.mean(feature) # Calculate mean
        std = np.std(feature)   # Calculate std
        feature = (feature - mean)/std
        train_X[:,i] = feature
        test_X[:,i] = (test_X[:,i] - mean)/std
    
    grad_threshold = 1e-3
    #for eta in [0.01, 0.1, 1, 4, 7, 7.5, 7.6]:
    for eta in [0.00001]:
        # Run logistic regression on train data
        train_start = time.time()
        t, w, e_in = logistic_reg(train_X, train_y, w_init, max_its, eta, grad_threshold)
        train_end = time.time()
    
        # Find binary error on both test and train data
    
        test_binary_error = find_binary_error(w, test_X, test_y)
        train_binary_error = find_binary_error(w, train_X, train_y)

        print("Traing time: " + str(train_end-train_start))
        print("Traing iterations: " + str(t))
        print("In-sample error:" + str(e_in))
        print("Train binary error: " + str(train_binary_error))
        print("Test binary_error: " + str(test_binary_error))
        print("\n") # Make 2 empty lines
    

if __name__ == "__main__":
    main()