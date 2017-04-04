


def read():
    Matrix1 = []
    Matrix2 = []

    f1 = open("C:\Users\jdbba\OneDrive\Documents\School\Research\Tests\Python Modularity Testing\dipha\Test\diag1.txt")
    f2 = open("C:\Users\jdbba\OneDrive\Documents\School\Research\Tests\Python Modularity Testing\dipha\Test\diag2.txt")

    next = f1.readline().strip()
    while next != "":
        arr = next.split()
        Matrix2.append(arr)
        next = f1.readline()

    next2 = f2.readline().strip()
    while next2 != "":
        arr2 = next2.split()
        Matrix2.append(arr2)
        next2 = f2.readline()
