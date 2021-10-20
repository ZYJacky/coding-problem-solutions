#!/usr/bin/python3
# Homework 5 Code
from sklearn.tree import DecisionTreeClassifier
import numpy as np
import matplotlib.pyplot as plt
import math

def adaboost_trees(X_train, y_train, X_test, y_test, n_trees):
    # %AdaBoost: Implement AdaBoost using decision trees
    # %   using information gain as the weak learners.
    # %   X_train: Training set
    # %   y_train: Training set labels
    # %   X_test: Testing set
    # %   y_test: Testing set labels
    # %   n_trees: The number of trees to use

    num_train_data = len(X_train)
    num_test_data = len(X_test)

    # initial weight
    weight = [1/num_train_data] * num_train_data

    train_error = []
    test_error = []
    train_predictions = [0] * len(X_train)
    test_predictions = [0] * len(X_test)
    
    clf = DecisionTreeClassifier(criterion="entropy", max_depth=1)

    for iteration in range(n_trees):

        clf.fit(X_train, y_train, sample_weight=weight)

        current_train_prediction = [clf.predict(X_train)][0]
        current_test_prediction =  [clf.predict(X_test)][0]

        # Calculate E_in^(D_t)
        E_in = 0
        for i in range(len(X_train)):
            if(y_train[i] != current_train_prediction[i]):
                E_in += 1 * weight[i]

        # Calculate y
        y = math.sqrt((1 - E_in)/E_in)

        # Calculate a_t
        a_t = 0.5 * math.log((1 - E_in)/E_in)

        # Calculate z_t
        z_t = y * E_in + (1/y) * (1 - E_in)

        # Update weight
        for i in range(len(X_train)):
            if(y_train[i] == current_train_prediction[i]):
                weight[i] = (1/z_t) * weight[i] * math.exp(-a_t)
            else:
                weight[i] = (1/z_t) * weight[i] * math.exp(a_t)

        # Calculate train error
        current_train_error = 0
        train_predictions += (current_train_prediction - 4) * a_t
        for j in range(len(y_train)):
            if((np.sign(y_train[j] - 4)) != np.sign(train_predictions[j])):
                current_train_error += 1
        train_error.append(current_train_error/len(y_train))


        # Calculate test error
        current_test_error = 0
        test_predictions += (current_test_prediction - 4) * a_t
        for k in range(len(y_test)):
            if(np.sign(y_test[k] - 4) != np.sign(test_predictions[k])):
                current_test_error += 1
        test_error.append(current_test_error/len(y_test))
        
    return train_error, test_error


def main_hw5():
    # Load data
    og_train_data = np.genfromtxt('zip.train')
    og_test_data = np.genfromtxt('zip.test')

    train_13, test_13, train_35, test_35 = [],[],[],[]

    train_13 = np.array([row for row in og_train_data if (row[0] == 1 or row[0] == 3)])
    train_35 = np.array([row for row in og_train_data if (row[0] == 3 or row[0] == 5)])
    test_13 = np.array([row for row in og_test_data if (row[0] == 1 or row[0] == 3)])
    test_35 = np.array([row for row in og_test_data if (row[0] == 3 or row[0] == 5)])

    num_trees = 200

    # Split data
    X_train = train_35[:,1:] 
    y_train = train_35[:,0]
    X_test = test_35[:,1:] 
    y_test = test_35[:,0]

    train_error, test_error = adaboost_trees(X_train, y_train, X_test, y_test, num_trees)

    all_trees = list(range(1, 201))

    print(train_error[199])

    plt.plot(all_trees, train_error, label="Training Error")
    plt.plot(all_trees, test_error, label="Test Error")
    plt.title("3 vs 5 Training and Test error as a Function of the Number of Weak Hypotheses")
    plt.xlabel("Number of Weak Hypotheses")
    plt.ylabel("3 vs 5 Classification Error")
    plt.legend()
    plt.show()
    

if __name__ == "__main__":
    main_hw5()