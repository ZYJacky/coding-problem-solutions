#!/usr/bin/python3
# Homework 4 Code
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import cross_val_score
from scipy.stats import mode
import numpy as np
import matplotlib.pyplot as plt

def single_decision_tree(train, test):

    test_error = 0
    train_error = 0

    clf = DecisionTreeClassifier(criterion="entropy")
    clf = clf.fit(train[:,1:], train[:,0])

    predictions = clf.predict(train[:,1:])
    for i in range(len(train)):
        if predictions[i] != train[i][0]:
            train_error+=1

    predictions = clf.predict(test[:,1:])
    for i in range(len(test)):
        if predictions[i] != test[i][0]:
            test_error+=1

    test_error = test_error/len(test)
    train_error = train_error/len(train)

    return train_error, test_error


def bagged_tree(train, test, num_bags):
    # The `bagged_tree` function learns an ensemble of numBags decision trees 
    # and also plots the  out-of-bag error as a function of the number of bags
    #
    # % Inputs:
    # % * `X_train` is the training data
    # % * `y_train` are the training labels
    # % * `X_test` is the testing data
    # % * `y_test` are the testing labels
    # % * `num_bags` is the number of trees to learn in the ensemble
    #
    # % Outputs:
    # % * `out_of_bag_error` is the out-of-bag classification error of the final learned ensemble
    # % * `test_error` is the classification error of the final learned ensemble on test data
    #
    # % Note: You may use sklearns 'DecisonTreeClassifier'
    # but **not** 'RandomForestClassifier' or any other bagging function

    all_test_error = []
    all_oob_error = []

    # [0, ..., len(train)-1], used in setdiff1d
    all_rows = list(range(0, len(train)))

    # Record the OOB prediction of each bagged tree for final agregation
    OOB = []
    for i in range(len(train)):
        OOB.append([])

    # Record the test prediction of each bagged tree
    test_result = []
    for i in range(len(test)):
        test_result.append([])

    #np.random.seed(153)
    clf = DecisionTreeClassifier(criterion="entropy")
    for i in range(num_bags):

        out_of_bag_error = 0
        test_error = 0

        # Sample with replacements the data points and record the missing ones
        rows = np.random.choice(train.shape[0], len(train), replace=True)
        missing_rows = np.setdiff1d(all_rows, rows)

        # Generate the actual data sets according to previous sampling
        D = train[rows,:]

        X_D = D[:,1:]
        y_D = D[:,0]

        # Learn the decision tree
        clf.fit(X_D, y_D)

        # get the predicted value of this tree on missing rows
        for row in missing_rows:
            OOB[row].append(clf.predict([train[row,1:]])[0])
        
        for i in range(len(test_result)):
            test_result[i].append(clf.predict([test[i,1:]])[0])

        # Aggregate the prediction of bagged trees on OOB values
        S = 0
        for i in range(len(OOB)):
            if OOB[i] != []:  # at least out of bag in one data set
                S+=1
                if mode(OOB[i])[0][0] != train[i][0]:
                    out_of_bag_error+=1
    
        # Aggregate the prediction of each bagged tree on test result
        majority_test_result = mode(test_result, axis=1)
        for i in range(len(test_result)):
            if(majority_test_result[0][i][0] != test[i][0]):
                test_error+=1

        test_error = test_error/len(test)
        out_of_bag_error = out_of_bag_error/S

        all_oob_error.append(out_of_bag_error)
        all_test_error.append(test_error)

    return all_oob_error, all_test_error

def main_hw4():
    # Load data
    og_train_data = np.genfromtxt('zip.train')
    og_test_data = np.genfromtxt('zip.test')

    train_13, test_13, train_35, test_35 = [],[],[],[]

    for line in og_train_data:
        if(line[0] == 1 or line[0] == 3):
            train_13.append(line)
        if(line[0] == 3 or line[0] == 5):
            train_35.append(line)

    for line in og_test_data:
        if(line[0] == 1 or line[0] == 3):
            test_13.append(line)
        if(line[0] == 3 or line[0] == 5):
            test_35.append(line)

    
    train_13 = np.array(train_13)
    test_13 = np.array(test_13)
    train_35 = np.array(train_35)
    test_35 = np.array(test_35)

    train_error, test_error = single_decision_tree(train_13, test_13)
    print("Train error of single decision tree 1 vs 3:" + str(train_error))
    print("Test error of single decision tree 1 vs 3:" + str(test_error))

    train_error, test_error = single_decision_tree(train_35, test_35)
    print("Train error of single decision tree 3 vs 5:" + str(train_error))
    print("Test error of single decision tree 3 vs 5:" + str(test_error))

    all_test_error = []
    all_oob_error = []
    bags = []

    num_bags = 200

    bags = list(range(1, 201))

    all_oob_error, all_test_error = bagged_tree(train_13, test_13, num_bags)  
    
    print("Test error of 200 bagged:" + str(all_test_error[199]))
    print("OOB error of 200 bagged:" + str(all_oob_error[199]))
    plt.plot(bags, all_oob_error)
    plt.title("1 vs 3 OOB error as a function of number of bags")
    plt.xlabel("number of bags")
    plt.ylabel("1 vs 3 OOB error")
    plt.show()

    plt.plot(bags, all_test_error)
    plt.title("1 vs 3 Test error as a function of number of bags")
    plt.xlabel("number of bags")
    plt.ylabel("1 vs 3 Test error")
    plt.show()

    all_oob_error, all_test_error = bagged_tree(train_35, test_35, num_bags)  
    
    print("Test error of 200 bagged:" + str(all_test_error[199]))
    print("OOB error of 200 bagged:" + str(all_oob_error[199]))
    plt.plot(bags, all_oob_error)
    plt.title("3 vs 5 OOB error as a function of number of bags")
    plt.xlabel("number of bags")
    plt.ylabel("3 vs 5 OOB error")
    plt.show()

    plt.plot(bags, all_test_error)
    plt.title("3 vs 5 Test error as a function of number of bags")
    plt.xlabel("number of bags")
    plt.ylabel("3 vs 5 Test error")
    plt.show()


  

if __name__ == "__main__":
    main_hw4()
