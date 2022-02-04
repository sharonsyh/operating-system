Pfind 
	- text searching program that finds all lines that contain keywords given from the user under certain directory

	                                                            fork+execute
Manager.c                                                        ---------->     Worker.c * num of input process

1. write first task(commandline directory) on the first channel                  2. read the task from the first channel
5. read the result from the second channel                                       3. do the task and write the result(subtasks+# of searched dir/file/lines) on second channel
6. tokenize the result string                                                    4. print found lines that matches keyword (considering -a,-c options)
7. write found subtasks on first channel
8. update results
	
														repeat 2-8 until there's no more tasks to handle
														+ when the user press CTRL+C, the program exits, prints the result

presentation video LINK 
https://drive.google.com/file/d/1CTnTpy595GSPAUBmOhWksrdZAJ4iR7ID/view?usp=sharing

Quick Running video LINK
https://drive.google.com/file/d/11eJsRT4tRNEfBV9NSKgSFXmGq6296L5U/view?usp=sharing
