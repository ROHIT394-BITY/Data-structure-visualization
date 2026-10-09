## Data-structure-visualizer


# Motivation
Almost every computer science student agrees that Data Structures and Algorithms is one of the hardest subjects to get a good grip on. A big reason for this is that most of the concepts—like how pointers shift around in a linked list or how nodes get rearranged in an AVL tree—happen entirely in memory, so you have to mentally imagine everything.
Textbooks and slide decks usually just show a diagram of the start and the end, which skips the actual steps where the confusion happens. Debuggers don't help much either because scrolling through lists of variable values and memory addresses gets overwhelming fast. Most of us end up relying on random YouTube videos or drawing nodes by hand on scrap paper to make sense of what's going on.
I wanted to build a Data Structure Visualizer to make this whole process a lot less frustrating. The goal is to make a tool where you can type in your own data and actually watch the operations play out step-by-step. Being able to slow down the animation, pause, or step back when you get lost makes it way easier to build a clear mental picture, especially when studying for lab exams or coding interviews.


# Features 
Visualizing the Essentials:Covers the main structures we struggle with the most—like Linked Lists, Stacks, Queues, Binary Search Trees, and Graphs.
The visual canvas updates automatically whenever you insert, delete, or search for a node, so you can actually see the structure changing shape.
Play, Pause, and Step Through: You can pause the animation anytime or click step-by-step through the algorithm to see exactly what changed in that split second.
Speed Slider: Lets you speed through the simple stuff or slow things down to a crawl when tricky operations like tree rotations happen.
Custom Inputs & Random Generator:You aren't stuck with fixed hardcoded data—you can type in your own custom numbers or nodes to test edge cases.
Has a quick Randomize button if you just want to generate test data instantly.
Side-by-Side Code Highlighting:Displays matching pseudocode (or standard C++/Python snippet) right next to the canvas.
Highlights the exact line running at that moment, which makes it way easier to connect actual code logic to the visual movement.
Simple Step Explanations:A small log box at the bottom that gives plain-English notes on what's going on (like "Moving head pointer to Node 4" or "Node 12 is unbalanced, starting left rotation").

Runs Right in the Browser:

Built as a web application, so there’s zero setup required—no installing libraries, compilers, or extra software to run it.
