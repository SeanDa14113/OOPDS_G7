/***********************************************************************
CCP6124 Object-oriented Programming and Data Structures
Trimester 2610
Project: Virtual Machine and Assembly Language Interpreter

Tutorial: TT8L
Group: 7

Member 1: 252UC2543V, Wong Haw Jack, WONG.HAW.JACK@student.mmu.edu.my
Member 2: 252UC2546J, Chen Chee Chuen, CHEN.CHEE.CHUEN@student.mmu.edu.my
Member 3: 252UC2528G, Tan Yi Da, TAN.YI.DA@student.mmu.edu.my
Member 4: 253UC256L3, Tan Khai Yu, TAN.KHAI.YU@student.mmu.edu.my

This program strictly follow below program structure:
- Custom Data Structures
- Virtual Machine Architecture
- Assembly Language Runner (Interpreter)
- Runner

************************************************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

//*****************************************************************
//              SECTION 1: CUSTOM DATA STRUCTURES
//*****************************************************************
// 1.1: SYSTEM VECTOR
// Written by: Tan Khai Yu

template <class DataType>
class MyVector
{
private:
    DataType* arr;     // Box to hold our actual items
    int capacity;      // Total space we opened in total
    int current;       // Spaces we actually used right now

    // Grow the box size when it gets full
    void resize()
    {
        int NewCapacity;

        if (capacity == 0)
        {
            NewCapacity = 1;
        }
        else
        {
            NewCapacity = capacity * 2;
        }

        DataType* t = new DataType[NewCapacity];

        for (int i = 0; i < current; i++)
        {
            t[i] = arr[i];
        }

        delete[] arr;
        arr = t;
        capacity = NewCapacity;
    }

public:
    // Setup empty values at the start
    MyVector()
    {
        arr = nullptr;
        capacity = 0;
        current = 0;
    }

    // Clear memory when program closes
    ~MyVector()
    {
        if (arr != nullptr)
        {
            delete[] arr;
        }
    }

    // Put a new item to the back
    void push_back(DataType data)
    {
        if (current == capacity)
        {
            resize();
        }
        arr[current] = data;
        current++;
    }

    // Read data using standard brackets vec[i]
    DataType& operator[](int i)
    {
        return arr[i];
    }

    // Return total items inside
    int size() const
    {
        return current;
    }
};

// 1.2: SYSTEM STACK
// Written by: Tan Khai Yu

class MyStack
{
private:
    signed char storage[8]; // Fixed array to hold 8 bytes as required
    int topIndex;           // Tracks the top item position

public:
    // Set stack as empty at first
    MyStack()
    {
        topIndex = -1;
    }

    // Put a new byte on top of the stack
    void push(signed char val)
    {
        if (topIndex < 7)
        {
            topIndex++;
            storage[topIndex] = val;
        }
    }

    // Take the top item off the stack
    signed char pop()
    {
        // Crash program if try to pop empty stack as required
        if (topIndex < 0)
        {
            std::cerr << "Stack Underflow Error!" << std::endl;
            exit(1);
        }

        signed char val = storage[topIndex];
        topIndex--;
        return val;
    }

    int get_top_index() {return topIndex;}
};
// 1.3: SYSTEM QUEUE
// Written By: Chen Chee Chuen
//Custom generic Queue using dynamic array. Implements FIFO (First In First Out) operations.

template <class T>
class MyQueue
{
private:
    T* data;  // pointer to the dynamic array (storing queue elements)
    int frontIndex;  // index of first element
    int rearIndex; // index of last element
    int capacity; // current array capacity / maximum number of elements the array can store
    void resize()
        {
            int newCapacity;  // store the new array size

            if (capacity == 0)
                newCapacity = 1;   // first allocation, create array of size 1
            else
                newCapacity = capacity * 2;   // double the current capacity (e.g: 4 to 8)

            T* temp = new T[newCapacity];  // Create a new array with a larger capacity to store more elements

            int j = 0;  // Index for the new array

            for (int i = frontIndex; i <= rearIndex; i++)   // Copy all queue elements
            {
                temp[j] = data[i];  // copy one element
                j++;   // move to the next position
            }

            delete[] data;   //delete the old array to free the old meomory

            data = temp; // data now points to the new array

            frontIndex = 0;  // reset front to index 0

            rearIndex = j - 1;  // rear becomes the last copied element

            capacity = newCapacity;  //update the new capacity
        }

public:
    MyQueue()  //constructor
    {
        data = nullptr;   //no memory allocated yet
        frontIndex = 0;  //front start at index 0
        rearIndex = -1;  //no element exists (queue is empty)
        capacity = 0;  //no storage yet
    }

    ~MyQueue()  //destructor
    {
        delete[] data;  //release dynamic memory - we must free it, otherwise memory Leak.
    }

    bool empty()
    {
        return frontIndex > rearIndex;  //true if no element remain
    }

    int size()
    {
        return rearIndex - frontIndex + 1;  //calculate number of elements
    }

    void enqueue (T value)
    {
        if (rearIndex + 1 >= capacity)  //check whether there is enough space - is the array full?
            resize();  //if yes, create a bigger array

        rearIndex++;  //move rear to the next position

        data[rearIndex] = value;  //store the new element
    }

    T dequeue()
    {
        if (empty())
        {
            std::cout << "Queue is empty!" << std::endl;  //display error message
            exit(1);  //stop the program
        }
        return data[frontIndex++];  //return the front element, then move frontIndex forward(Take the first item and remove it from the queue)
    }

    T front()
    {
        if (empty())  // check if queue is empty
        {
            std::cout << "Queue is empty!" << std::endl;  //display error message
            exit(1);  // stop the program
        }

        return data[frontIndex];  //return front element without removing it (Look at the first item only (don't remove it))
    }
};

//*****************************************************************
//             SECTION 2: VIRTUAL MACHINE ARCHITECTURE
//*****************************************************************
//
// 2.1: Register
// Written by: Tan Khai Yu
// Base class register
class Register
{
protected:
    signed char value; // 1-byte register data slot

public:
    // Setup starting value to 0
    Register()
    {
        value = 0;
    }

    virtual ~Register() {}

    // Method to update register data
    void setValue(signed char val)
    {
        value = val;
    }

    // Method to read register data
    signed char getValue() const
    {
        return value;
    }
};

// Derived class for R0-R7 registers
class GeneralRegister : public Register {};

class StackIndexRegister : public Register
{
public:
    StackIndexRegister()
    {
        value = 0;

    }
    void push()
    {
        value++;
    }
    void pop()
    {
        value--;
    }
};

// 2.2: Program Counter
// Written by: Wong Haw Jack
class ProgramCounter {
private:
    unsigned char value; // 1-byte wide tracking counter
public:
    ProgramCounter() : value(0) {} // Initialize the counter by 0
    void increment() { value++; } // Increment by 1
    // Setter-getter function for program counter
    void setValue(unsigned char val) { value = val; }
    unsigned char getValue() const { return value; }
};

// 2.3: Flags
// Written by: Tan Khai Yu
// Manages CF, ZF, UF, OF bits
class FlagRegister
{
private:
    bool CF; // Carry Flag
    bool ZF; // Zero Flag
    bool UF; // Underflow Flag
    bool OF; // Overflow Flag

public:
    // Reset all flags to false initially
    FlagRegister()
    {
        CF = false;
        ZF = false;
        UF = false;
        OF = false;
    }

    // Update methods for flags
    void setCF(bool b) { CF = b; }
    void setZF(bool b) { ZF = b; }
    void setUF(bool b) { UF = b; }
    void setOF(bool b) { OF = b; }

    // Read methods for checking flags
    bool getCF() const { return CF; }
    bool getZF() const { return ZF; }
    bool getUF() const { return UF; }
    bool getOF() const { return OF; }
};

// 2.4: Memory(64 BYTES)
// Written by: Tan Khai Yu

// Handles storage over 64 bytes
class Memory
{
private:
    signed char storage[64]; // Fixed size 64-byte array

public:
    // Fill all memory space with 0 at first
    Memory()
    {
        for (int i = 0; i < 64; i++)
        {
            storage[i] = 0;
        }
    }

    // Write data to an address
    void write(int address, signed char val)
    {
        if (address >= 0 && address < 64)
        {
            storage[address] = val;
        }
    }

    // Read data from an address
    signed char read(int address) const
    {
        if (address >= 0 && address < 64)
        {
            return storage[address];
        }
        return 0;
    }
};

//2.5 CPU
//Written by: Wong Haw Jack
class CPU {
private:
    GeneralRegister r[8]; // We create an array to store 8 register and access the register using index 0-7
    Memory cpu_memory; // Main 64-byte memory.

    FlagRegister* ptr_flag = new FlagRegister(); // Flag register aggregated by CPU.
    ProgramCounter pc; // CPU Program counter
    MyStack stack; // CPU stack
    StackIndexRegister stack_index; // Incremented when items are pushed, decremented when an item is popped from the stack.

public:
    CPU() : stack_index() {}

    ~CPU() {delete ptr_flag;}

    signed char readMemory(int address) // Accessor for getting data from
    {
        return cpu_memory.read(address);
    }

    void writeMemory(int address, signed char value)
    {
        cpu_memory.write(address, value);
    }

    FlagRegister* getFlags()
    {
        return ptr_flag;
    }

    signed char getReg(int idx) const {
        if (idx >= 0 && idx < 8)
            return r[idx].getValue();
        return 0;
    }
    void setReg(int idx, signed char val) {
        if (idx >= 0 && idx < 8)
            r[idx].setValue(val);
    }


    void pushStack(signed char val) {
        stack.push(val);
        stack_index.push();
    }
    signed char popStack() {
        if (stack_index.getValue() == 0) {
            std::cerr << "Stack Underflow Error!" << std::endl;
            exit(1);
        }
        stack_index.pop();
        return stack.pop();
    }
    unsigned char getPC() const { return pc.getValue(); } // Function for getting data in program counter.
    void incrementPC() { pc.increment(); } // Function for update program counter.
    void setPC(unsigned char val) { pc.setValue(val); } // Function for setting program counter.
};
// Forward declaration;
//*****************************************************************
//            SECTION 3: Assembly Language Interpreter
//*****************************************************************
// 3.1: Instruction (Base Class)
// Written by: Tan Khai Yu
// Abstract base class for commands
class Instruction
{
public:
    virtual ~Instruction() {}

    // Pure virtual function for polymorphism
    virtual void execute(CPU& cpu) = 0;
};

// 3.2: Input Operations
// 3.3: Output Operations
// Written by: Wong Haw Jack
class IOInstruction: public Instruction //Base Class for INPUT and OUTPUT.
{
public:
    int register_index; // Store the target register index
};

class Input: public IOInstruction
{
public:
    Input(int r) // Constructor: store the target register.
    {
        register_index = r;
    }
    void execute(CPU& cpu) override
    {
        int input;
        std::cout << "?" << std::endl;
        std::cin >> input; // Get the integer from user
        cpu.setReg(register_index, static_cast<signed char>(input)); // Static cast the integer to signed char.
        // Update the flag. If user input > 127, it is overflow. If < -128, it is underflow. If == 0 then zero flag is raise.
        if(input > 127) {cpu.getFlags()->setOF(true);}
        else if(input < -128) {cpu.getFlags()->setUF(true);}
        else if(input == 0) {cpu.getFlags()->setZF(true);}
    }

};

class Display: public IOInstruction
{
public:
    Display(int r) // Constructor to store the target register.
    {
        register_index = r;
    }
    void execute(CPU& cpu) override
    {
        std::cout << int(cpu.getReg(register_index)) << endl; // Display the target register value.
    }
};
// 3.4: MOV Operations
// Written by: Tan Yi Da
class MovOperation : public Instruction
{
private:
    int mov_mode; // MOV mode (0: R1,10 / 1: R1,R0 / 2: R1,[R2])
    int destination;
    int source;
    signed char number;

public:

    MovOperation(int dest, signed char num)
    {
        mov_mode = 0;
        destination = dest;
        number = num;
        source = 0;
    }

    // Mode 1: MOV R1, R0 (Copies the value stored in the source register to the destination register)
    MovOperation(int dest, int src)
    {
        mov_mode = 1;
        destination = dest;
        source = src;
        number = 0;
    }

    // Mode 2: MOV R1, [R2] (Reads data from the memory address stored in the source register and copies it to the destination register)
    // An extra dummy parameter (int mode) is added to differentiate the overload from the two-argument constructor
    MovOperation(int dest, int src, int mode)
    {
        mov_mode = 2;
        destination = dest;
        source = src;
        number = 0;
    }

    void execute(CPU& cpu) override
    {
        switch(mov_mode)
        {
            case 0:
                cpu.setReg(destination, number);
                break;
            case 1:
                cpu.setReg(destination, cpu.getReg(source));
                break;
            case 2:
                {
                    // Fetch the memory address value stored inside the source register
                    int address = static_cast<int>(cpu.getReg(source));
                    if (address >= 0 && address < 64)
                    {
                        cpu.setReg(destination, cpu.readMemory(address));
                    }
                    else
                    {
                        std::cerr << "Memory Access Error in MOV Instruction!" << std::endl;
                    }
                }
                break;
            default:
                std::cout << "You can't MOVE this!!!" << std::endl;
                return;
        }
    }
};
// 3.5: Arithmetic Operations
// Written by: Wong Haw Jack
class Arithmetic: public Instruction // Base class for ADD, SUB, MUL and DIV.
{
private:
    int source_reg; // Store the data of source register index.
    int destination_reg; // Store the data of destination index
public:
    Arithmetic(int d, int s): destination_reg(d), source_reg(s){} //Constructor to initialize the data when any arithmetic instruction run.
    // Accessor of the source and destination register.
    int get_source_reg() const {return source_reg;}
    int get_destination_reg() const {return destination_reg;}
};

class ADD: public Arithmetic
{
public:
    ADD(int d, int s): Arithmetic(d, s){} // When the instruction is run, the input is automatically pass to the Arithmetic constructor.

    void execute(CPU& cpu)
    {
        //First, we access the arithmetic destination and source index.
        int dest_index = get_destination_reg();
        int source_index = get_source_reg();
        //Then, we get the value from the cpu register using the destination and source index.
        signed char source_value = cpu.getReg(source_index);
        signed char dest_value = cpu.getReg(dest_index);
        // Then we get the signed char result by adding them together.
        // Although source_value and dest_value is signed char, when we do addition, it will become int.
        // So here static_cast the result to signed char again.
        signed char signed_result = static_cast<signed char>(source_value + dest_value);
        //Convert from signed char to unsigned char, then convert to int and add it up.
        //If the number higher than 255, then carry bit occur.
        int unsigned_result = static_cast<int>(static_cast<unsigned char>(source_value)) +
                   static_cast<int>(static_cast<unsigned char>(dest_value));
        bool overflowOccurred = (source_value > 0 && dest_value > 0 && signed_result < 0); //Addition of two positive number resultant negative means overflow occur.
        bool underflowOccurred = (source_value < 0 && dest_value < 0 && signed_result >= 0); //Addition of two negative number resultant positive means underflow occur.
        //Update the flag accordingly
        cpu.getFlags()->setCF(unsigned_result > 255);
        cpu.getFlags()->setUF(underflowOccurred);
        cpu.getFlags()->setOF(overflowOccurred);
        cpu.getFlags()->setZF(signed_result == 0);
        //Update the destination value at the end.
        cpu.setReg(dest_index, static_cast<signed char>(signed_result));
    }
};

class SUB: public Arithmetic
{
public:
    // When the instruction is run, the input is automatically pass to the Arithmetic constructor.
    SUB(int d, int s): Arithmetic(d, s){}
    void execute(CPU& cpu)
    {
        //First, we access the arithmetic destination and source index.
        int source_index = get_source_reg(); //Get the index of the register.
        int dest_index = get_destination_reg();
        //Then, we get the value from the cpu register using the destination and source index.
        signed char source_value = cpu.getReg(source_index); //Get the value through cpu member function
        signed char dest_value = cpu.getReg(dest_index);
        // Then we get the signed char result by adding them together.
        // Although source_value and dest_value is signed char, when we do addition, it will become int.
        // So here static_cast the result to signed char again.
        signed char signed_result = static_cast<signed char>(dest_value - source_value);
        //Convert from signed char to unsigned char, then convert to int and add it up.
        int unsigned_result = static_cast<int>(static_cast<unsigned char>(dest_value)) - static_cast<int>(static_cast<unsigned char>(source_value));
        int unsigned_source = static_cast<int>(static_cast<unsigned char>(source_value));
        bool overflowOccurred = (dest_value > 0 && source_value < 0 && signed_result < 0); //Subtraction of two positive number resultant positive means overflow occur.
        bool underflowOccurred = (dest_value < 0 && source_value > 0 && signed_result >= 0); //Subtraction of two positive number resultant negative means underflow occur.
        bool carryOccured = (unsigned_result < unsigned_source); // Carry flag is raise when unsigned_result < unsigned source.
        //Update the flag accordingly
        cpu.getFlags()->setCF(carryOccured);
        cpu.getFlags()->setUF(underflowOccurred);
        cpu.getFlags()->setOF(overflowOccurred);
        cpu.getFlags()->setZF(signed_result == 0);
        //Update the destination value at the end.
        cpu.setReg(dest_index, static_cast<signed char>(signed_result));
    }
};

class MUL: public Arithmetic
{
public:
    MUL(int d, int s): Arithmetic(d, s){}

    void execute(CPU& cpu)
    {
        int source_index = get_source_reg();
        int dest_index = get_destination_reg();

        signed char source_value = cpu.getReg(source_index);
        signed char dest_value = cpu.getReg(dest_index);

        signed char signed_result = static_cast<signed char>(dest_value * source_value); // Cast operands to unsigned values first, then store their subtract inside a wider 32-bit int

        int unsigned_result = static_cast<int>(static_cast<unsigned char>(dest_value)) * static_cast<int>(static_cast<unsigned char>(source_value));

        bool overflowOccurred = (int(dest_value) * int(source_value) > 127); //In byte, any multiplication make number larger than 127. it is overflow.
        bool underflowOccurred = (int(dest_value) * int(source_value) <= -128); //else, it is underflow.

        cpu.getFlags()->setCF(unsigned_result > 255);
        cpu.getFlags()->setUF(underflowOccurred);
        cpu.getFlags()->setOF(overflowOccurred);
        cpu.getFlags()->setZF(signed_result == 0);

        cpu.setReg(dest_index, static_cast<signed char>(signed_result));
    }
};

class DIV: public Arithmetic
{
public:
    DIV(int d, int s): Arithmetic(d, s){}

    void execute(CPU& cpu)
    {
        int source_index = get_source_reg();
        int dest_index = get_destination_reg();

        signed char source_value = cpu.getReg(source_index);
        signed char dest_value = cpu.getReg(dest_index);

        if (source_value == 0) {
            std::cerr << "Runtime Error: Division by zero!" << std::endl;
            return;
        }

        signed char signed_result = static_cast<signed char>(dest_value / source_value); // Cast operands to unsigned values first, then store their subtract inside a wider 32-bit int

        int unsigned_result = static_cast<int>(static_cast<unsigned char>(dest_value)) / static_cast<int>(static_cast<unsigned char>(source_value));
        //Division of two number only make the resultant closer to 0, so it is impossible to overflow.
        //Unless it -128 / -1 = 128.
        bool overflowOccurred = (dest_value == -128 && source_value == -1);
        //underflow dont occur during division since division bring the result closer to 0.

        cpu.getFlags()->setCF(unsigned_result > 255);
        cpu.getFlags()->setOF(overflowOccurred);
        cpu.getFlags()->setZF(signed_result == 0);

        cpu.setReg(dest_index, static_cast<signed char>(signed_result));
    }
};
// 3.6: Increment and Decrement Operations
// Written by: Chen Chee Chuen
class INC : public Instruction
{
private:
    int reg;

public:
    INC(int r)
    {
        reg = r;
    }

    void execute(CPU& cpu) override
    {
        int result = cpu.getReg(reg) + 1;

        // Reset Flags
        cpu.getFlags()->setOF(false);
        cpu.getFlags()->setUF(false);
        cpu.getFlags()->setZF(false);
        cpu.getFlags()->setCF(false);

        // Overflow
        if(result > 127)
        {
            cpu.getFlags()->setOF(true);
            cpu.getFlags()->setCF(true);
        }

        // Underflow
        if(result < -128)
        {
            cpu.getFlags()->setUF(true);
            cpu.getFlags()->setCF(true);
        }

        if((signed char)result == 0)
        {
            cpu.getFlags()->setZF(true);
        }

        cpu.setReg(reg, (signed char)result);
    }
};

class DEC : public Instruction
{
private:
    int reg;

public:
    DEC(int r)
    {
        reg = r;
    }

    void execute(CPU& cpu) override
    {
        int result = cpu.getReg(reg) - 1;

        // Reset Flags
        cpu.getFlags()->setOF(false);
        cpu.getFlags()->setUF(false);
        cpu.getFlags()->setZF(false);
        cpu.getFlags()->setCF(false);

        // Overflow
        if(result > 127)
        {
            cpu.getFlags()->setOF(true);
            cpu.getFlags()->setCF(true);
        }

        // Underflow
        if(result < -128)
        {
            cpu.getFlags()->setUF(true);
            cpu.getFlags()->setCF(true);
        }

        // Zero Flag
        if((signed char)result == 0)
        {
            cpu.getFlags()->setZF(true);
        }

        cpu.setReg(reg, (signed char)result);
    }
};

// 3.7: ROL Operations
// 3.8: ROR Operations
// Written by Wong Haw Jack
class Rotate : public Instruction // Base class for both rotate left and rotate right.
{
private:
    int destination_reg; // Private member to store the data of destination register.
    int count; // Private member to store the data of count.

public:
    Rotate(int d, int c)
    {
        destination_reg = d;
        count = c;
    }
    // Accessor function
    int get_dest_reg() const {return destination_reg;}
    int get_count() const {return count;}
};

class ROL : public Rotate
{
public:
    ROL(int d, int c) : Rotate(d, c){}

    void execute(CPU& cpu)
    {
        int destination_index = get_dest_reg();
        int count = get_count();
        //Modulo 8 to wrap around the input between 0-7.
        //example: input = 20, 20 % 8 = 4. Rotate left 4.
        count %= 8;
        //To perform rotate, we need to cast the the signed char to unsigned char first.
        signed char signed_before_ro = cpu.getReg(destination_index);
        unsigned char unsigned_before_ro = static_cast<unsigned char>(signed_before_ro);

        unsigned char unsigned_after_ro = 0;
        if (count == 0) //If count == 0, there is no rotation. The target value remain unchanged.
        {
            unsigned_after_ro = unsigned_before_ro;
        }
        else
        {
            //Else, it will perform the rotate.
            //example: 1011 0100 -> 0100 1011 (rotate 4)
            //First, shift the unsigned bit to the left by count = 4. 1011 0100 -> 0100 0000
            //Then, shift the unsigned bit to the right by 8 - count. 8 - 4 = 4.
            // 1011 0100 -> 0000 1011
            // Lastly, adding up together by using 'OR'
            // 0100 0000 or 0000 1011 -> 0100 1011
            unsigned_after_ro = (unsigned_before_ro << count) | (unsigned_before_ro >> (8 - count));
        }
        signed char signed_after_ro = static_cast<signed char>(unsigned_after_ro); //Finally, cast it back to signed char.

        cpu.setReg(destination_index, signed_after_ro);

        cpu.getFlags()->setZF(signed_after_ro == 0);
    }
};
class ROR : public Rotate
{
public:
    ROR(int d, int c) : Rotate(d, c){}

    void execute(CPU& cpu)
    {
        int destination_index = get_dest_reg();
        int count = get_count();

        count %= 8;

        signed char signed_before_ro = cpu.getReg(destination_index);
        unsigned char unsigned_before_ro = static_cast<unsigned char>(signed_before_ro);

        unsigned char unsigned_after_ro = 0;
        if (count == 0)
        {
            unsigned_after_ro = unsigned_before_ro;
        }
        else
        {
            //Same operation with rotate right it just shift right first, then shift left.
            unsigned_after_ro = (unsigned_before_ro >> count) | (unsigned_before_ro << (8 - count));
        }
        signed char signed_after_ro = static_cast<signed char>(unsigned_after_ro);

        cpu.setReg(destination_index, signed_after_ro);

        cpu.getFlags()->setZF(signed_after_ro == 0);
    }
};
// 3.9: Shift Operations
// Written By: Tan Yi Da
class SftOperation : public Instruction
{
private:
    int sft_mode; // 0: SHL (Shift Left), 1: SHR (Shift Right)
    int sft_num; // Number of shifts
    int destination; // Destination register index

public:
    SftOperation(int dest, int Count, int mode)
    {
        sft_mode = mode;
        destination = dest;
        sft_num = Count;
    }

    void execute(CPU& cpu) override
    {
        // Retrieve the original value of the register via the public interface
        signed char val = cpu.getReg(destination);

        switch(sft_mode)
        {
            case 0: // SHL: Logical Shift Left
                for(int i = 0; i < sft_num; i++)
                {
                    val = val << 1;
                }
                break;

            case 1: // SHR: Logical Shift Right
                // Casting to unsigned char ensures that the higher bits are padded with 0s during right shift (logical shift)
                unsigned char uVal = static_cast<unsigned char>(val);
                for(int i = 0; i < sft_num; i++)
                {
                    uVal = uVal >> 1;
                }
                val = static_cast<signed char>(uVal);
                break;
        }

        // Store the shifted value back to the register and update the Zero Flag (ZF)
        cpu.setReg(destination, val);
        cpu.getFlags()->setZF(val == 0);
    }
};



// 3.10: Load and Store Operations
// Written by: Chen Chee Chuen
class LOAD : public Instruction  //Direct LOAD // LOAD R1, [20]
{
private:
    int reg;
    int address;
public:
    LOAD(int r, int addr)
    {
        reg = r;
        address = addr;
    }

    void execute(CPU& cpu)override
    {
        if(address < 0 || address >= 64)
        {
            std::cout << "Memory Error" << std::endl;
            return;
        }

        cpu.setReg(reg, cpu.readMemory(address));
    }
};

class STORE : public Instruction  // Direct STORE
{
private:
    int reg;
    int address;

public:
    STORE(int r, int addr)
    {
        reg = r;
        address = addr;
    }

    void execute(CPU& cpu) override
    {
        if(address < 0 || address >= 64)
        {
            std::cout << "Memory Error" << std::endl;
            return;
        }

        cpu.writeMemory(address, cpu.getReg(reg));
    }
};

class LOAD_INDIRECT : public Instruction  // Indirect load  // LOAD R1, [R2]
{
private:
    int destReg;
    int addrReg;

public:
    LOAD_INDIRECT(int d, int a)
    {
        destReg = d;
        addrReg = a;
    }

    void execute(CPU& cpu) override
    {
        int address = cpu.getReg(addrReg);

        if(address < 0 || address >= 64)
        {
            std::cout << "Memory Error" << std::endl;
            return;
        }

        cpu.setReg(destReg, cpu.readMemory(address));
    }
};

class STORE_INDIRECT : public Instruction  // Indirect store
{
private:
    int addrReg;
    int sourceReg;

public:
    STORE_INDIRECT(int s, int a)
    {
        addrReg = a;
        sourceReg = s;
    }

    void execute(CPU& cpu) override
    {
        int address = cpu.getReg(addrReg);

        if(address < 0 || address >= 64)
        {
            std::cout << "Memory Error" << std::endl;
            return;
        }

        cpu.writeMemory(address, cpu.getReg(sourceReg));
    }
};

// 3.11: Flag Reset Instruction
// Written by: Tan Khai Yu
// Resets a specific flag (C, Z, U, or O) to false
class RESET : public Instruction
{
private:
    char FlagType;  // Flag to reset: 'C', 'Z', 'U', or 'O'

public:
    RESET(char Type) : FlagType(Type) {}

    void execute(CPU& cpu) override
    {
        FlagRegister* flags = cpu.getFlags();
        switch (FlagType)
        {
        case 'C': flags->setCF(false); break;
        case 'Z': flags->setZF(false); break;
        case 'U': flags->setUF(false); break;
        case 'O': flags->setOF(false); break;
        default: break;
        }
    }
};

// 3.12: Stack Operations
// Written by: Tan Khai Yu
// PUSH: Stores a register's value onto the stack
class PUSH : public Instruction
{
private:
    int Source_Reg;  // Register to push (0-7)

public:
    PUSH(int Reg) : Source_Reg(Reg) {}

    void execute(CPU& cpu)
    {
        signed char value = cpu.getReg(Source_Reg);
        cpu.pushStack(value);  // Push value onto stack
    }
};

// POP: Removes top stack value and stores it into a register
class POP : public Instruction
{
private:
    int Destination_Reg;  // Register to receive popped value (0-7)

public:
    POP(int Reg) : Destination_Reg(Reg) {}

    void execute(CPU& cpu)
    {
        signed char value = cpu.popStack();  // Pop from stack
        cpu.setReg(Destination_Reg, value);  // Store in register
    }
};

//*****************************************************************
//                       SECTION 4: Runner
//*****************************************************************
// 4.1: File Reader
// Written by: Chen Chee Chuen
// Reads an assembly(.asm) file line by line and storeseach instruction into the custom queue
class FileReader
{
public:
    void readFile(const std::string& filename, MyQueue<std::string>& instructionQueue)  //function: read file + store lines into queue
    {
        std::ifstream fin(filename);  //open file for reading (input file stream)

        if(!fin) //check if file failed to open
        {
            std::cout << "Error: Cannot open file" << std::endl;  //print error message
            return;  //stop function if file cannot be opened
        }

        std::string line;  //temporary variable to store each line from file

        while(getline(fin, line))  //read file line by line until EOF
        {
            if (line.empty())  //check if line is empty
            {
                continue;  //skip empty lines
            }

            instructionQueue.enqueue(line);  //store instruction into queue (FIFO order)
            //cout << "Debug: " << line << endl;
        }

        fin.close(); //close the file after reading and free the file resources
    }
};

// 4.2： Parser Class
//Writen by : Tan Yi Da

class Parser {
private:
    string line;
    string ins;

    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

public:
    Parser() {
        line = "";
        ins = "";
    }

    Instruction* parseLine(const string& inputLine) {
        size_t commentPos = inputLine.find(';');

        string cleanLine = inputLine;
        if(commentPos != string::npos){
            cleanLine = inputLine.substr(0, commentPos);
        }

        cleanLine = trim(cleanLine);

        if(cleanLine.empty()){
            return nullptr;
        }

        size_t spacePos = inputLine.find_first_of(" \t");
        string opcode = cleanLine.substr(0, spacePos);
        string args = trim(cleanLine.substr(spacePos + 1));

        //This is a safeguard if a opcode is not including in the rubric, the program will exit gracefully.
        if (opcode != "MOV" && opcode != "ADD" && opcode != "SUB" && opcode != "MUL" &&
            opcode != "DIV" && opcode != "INC" && opcode != "DEC" && opcode != "INPUT" &&
            opcode != "DISPLAY" && opcode != "SHL" && opcode != "SHR" && opcode != "ROL" &&
            opcode != "ROR" && opcode != "LOAD" && opcode != "STORE" && opcode != "PUSH" &&
            opcode != "POP" && opcode != "RESET") {
            std::cerr << "Invalid Instruction Encountered: " << opcode << "\n";
            std::exit(1); // Stop execution immediately to fulfill safety criteria
        }

        if (spacePos == string::npos) {
            string cmd = trim(cleanLine);
            if (cmd == "RESET") return new RESET(args[0]);
            return nullptr;
        }
        size_t commaPos = args.find(',');

        if (commaPos == string::npos) {
            if (opcode == "INPUT")   return new Input(args[1] - '0');
            if (opcode == "POP")     return new POP(args[1] - '0');
            if (opcode == "INC")     return new INC(args[1] - '0');
            if (opcode == "DEC")     return new DEC(args[1] - '0');

            if (opcode == "PUSH") {
                if (args[0] == 'R') return new PUSH(args[1] - '0');
                return new PUSH((signed char)stoi(args));
            }
            if (opcode == "DISPLAY") {
                if (args[0] == '[') return new Display(stoi(args.substr(1, args.length() - 2)));
                return new Display(args[1] - '0');
            }
            return nullptr;
        }

        string first = trim(args.substr(0, commaPos));
        string second = trim(args.substr(commaPos + 1));

        if (opcode == "ADD")   return new ADD(first[1] - '0', second[1] - '0');
        if (opcode == "SUB")   return new SUB(first[1] - '0', second[1] - '0');
        if (opcode == "MUL")   return new MUL(first[1] - '0', second[1] - '0');
        if (opcode == "DIV")   return new DIV(first[1] - '0', second[1] - '0');
        if (opcode == "ROL")   return new ROL(first[1] - '0', stoi(second));
        if (opcode == "ROR")   return new ROR(first[1] - '0', stoi(second));
        if (opcode == "SHL")   return new SftOperation(first[1] - '0', stoi(second), 0);
        if (opcode == "SHR")   return new SftOperation(first[1] - '0', stoi(second), 1);

        if (opcode == "MOV") {
            //cout << first << " " << second << endl;
            if (second[0] == 'R') return new MovOperation(first[1] - '0', second[1] - '0');
            if (second[0] == '[') return new MovOperation(first[1] - '0', second[2] - '0', 0);
            return new MovOperation(first[1] - '0', (signed char)stoi(second));
        }

        if (opcode == "LOAD")
        {
            int dstReg = first[1] - '0';
            // Check if second argument is indirect [Rn] or direct [address]
            if (second[1] == 'R')
            {
                // Remove the '[' and ']' characters before parsing
                string addrPart = second.substr(1, second.length() - 2);
                return new LOAD_INDIRECT(dstReg, addrPart[1] - '0');
            }
            // Otherwise it's a direct memory address (e.g., 10)
            else
                {
                    string memAddr = second.substr(1, second.length() - 2);
                    return new LOAD(dstReg, stoi(memAddr));
                }
        }

        if (opcode == "STORE")
            {
            int srcReg = first[1] - '0';
            // Check if second argument is indirect [Rn] or direct [address]
            if (second[0] == '[')
                {
                // Remove the '[' and ']' characters before parsing
                string addrPart = second.substr(1, second.length() - 2);
                return new STORE_INDIRECT(srcReg, addrPart[1] - '0');
                }
            // Otherwise it's a direct memory address (e.g., 10)
            else {return new STORE(srcReg, stoi(second));}
            }
        return nullptr;
        }
};

// 4.3: Runner
// Written by: Wong Haw Jack
class Runner {
private:
    MyVector<Instruction*> compiledInstructions;
    Parser lineParser;
    FileReader asmReader;

    // Helper to format 8-bit registers and memory outputs into 4-character padded blocks
    void formatAndPrintByte(std::ostream& os, signed char val) const {
        int intVal = static_cast<int>(val);
        if (intVal >= 0) {
            os << std::setw(4) << std::setfill('0') << intVal << "#";
        } else {
            // Replicates absolute sign value separation layout format safely
            os << "-" << std::setw(3) << std::setfill('0') << (-intVal) << "#";
        }
    }

    // Runner Memory Matrix Display
    void printMemoryToConsole(std::ostream& os, CPU& cpu) const {
        os << "#Memory#\n";
        for (int row = 0; row < 8; ++row){
            os << "#";
            for (int col = 0; col < 8; ++col){
                int address = (row * 8) + col;
                int memVal = static_cast<int>(cpu.readMemory(address));
                if(memVal >= 0){
                    os <<std::setw(4) << std::setfill('0') << memVal << "#";
                }
                else{
                    os << "-" << std::setw(3) << std::setfill('0') << (-memVal) << "#";
                }
            }
            os << "\n";
        }
    }

    //Virtual Machine Snapshot State Display
    void dumpStateToConsole(std::ostream& os, CPU& cpu,int pcValue) const {
        os << "#Begin#\n#Registers#";
        for(int i = 0; i < 8; ++i){
            int regVal = static_cast<int>(cpu.getReg(i));
            if (regVal >= 0){
                os << std::setw(4) << std::setfill('0') << regVal << "#";
            }
            else{
                os << "-" << std::setw(3) << std::setfill('0') << (-regVal) << "#";
            }
        }
        os << "\n";

        FlagRegister* flags = cpu.getFlags();
        os        << "#FLAGS#OF#" << flags -> getOF()
                  << "#UF#" << flags -> getUF()
                  << "#CF#" << flags -> getCF()
                  << "#ZF#" << flags -> getZF() << "#\n";

        os        << "#PC#" << std::setw(4) << std::setfill('0') << pcValue << "#\n";
        printMemoryToConsole(os, cpu);
        os        << "#End#\n";
    }

    void generateDump(std::ostream& os, CPU& cpu) const {
        dumpStateToConsole(os, cpu, cpu.getPC());
    }

public:
    Runner() {}

    ~Runner() {
        for (int i = 0; i < compiledInstructions.size(); i++) {
            delete compiledInstructions[i];
        }
    }

    bool loadProgram(const std::string& filename) {
        MyQueue<std::string> rawLinesQueue;
        asmReader.readFile(filename, rawLinesQueue);
        if (rawLinesQueue.empty()) return false;

        while (!rawLinesQueue.empty()) {
            std::string currentLine = rawLinesQueue.dequeue();
            Instruction* cmd = lineParser.parseLine(currentLine);
            if (cmd != nullptr) {
                compiledInstructions.push_back(cmd);
            }
        }
        return compiledInstructions.size() > 0;
    }

    void run(CPU& cpu, string program_file) {
        cpu.setPC(0); // Section 3 specs validation rule

        while (cpu.getPC() < compiledInstructions.size()) {
            int currentPC = cpu.getPC();
            Instruction* currentInstruction = compiledInstructions[currentPC];
            currentInstruction->execute(cpu);
            cpu.incrementPC();
        }

        // --- EXECUTOR DUMP TRIGGER POINT ---
        // 1. Output to Screen Console
        generateDump(std::cout, cpu);

        // 2. Output directly to the required text file dump on the drive
        string output_file = program_file.substr(0, program_file.length() - 4);
        output_file += "_result.txt";
        std::ofstream fout(output_file);
        if (fout) {
            generateDump(fout, cpu);
            fout.close();
        }
    }
};

int main(int argc, char* argv[])
{
//    std::cout << "========================================" << std::endl;
//    std::cout << "      VIRTUAL MACHINE RUNNER BOOT       " << std::endl;
//    std::cout << "========================================" << std::endl;

    //To check user provide file name.
    if (argc < 2) {
        std::cerr << "Error: Missing input file.\n";
        std::cerr << "Usage: " << argv[0] << " <filename.asm>\n";
        return 1;
    }
    //Extract the filename from the argument vector
    std::string programFile = argv[1];

    CPU myCpu;
    Runner vmInterpreter;

    //std::cout << "Loading: " << programFile << "..." << std::endl;

    if (!vmInterpreter.loadProgram(programFile)) {
        std::cerr << "Initialization Error: Unable to extract valid program instructions." << std::endl;
        return 1;
    }
    vmInterpreter.run(myCpu, programFile);

    return 0;
}
