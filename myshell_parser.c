#include "myshell_parser.h"
#include "stddef.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool isSymbol(const char c) {
	if (c == '&' || c == '|' || c == '<' || c == '>') {
		return true;
	} else {
		return false;
	}
}

char *read_next_token(const char **it) {
	static char token[MAX_LINE_LENGTH] = "";
	memset(token, 0, strlen(token));

	for (;**it != '\n' && **it != '\0'; ++*it) {
		if (**it == ' ') {
			if (strlen(token)) {
				++*it;
				return token;
			}
			continue;
		} else if (isSymbol(**it)) {
			if (strlen(token)) {
				return token;
			} else {
				strncat(token, *it, 1);
				++*it;
				return token;
			}
		} else {
			strncat(token, *it, 1);
		}
	}

	return token;
}

bool read_command(char **lex_str, int *index, struct pipeline_command **com, bool *isBkg) {
	int i = 0;

	(*com)->next = NULL;
	(*com)->redirect_in_path = NULL;
	(*com)->redirect_out_path = NULL;

	for (; lex_str[*index] != NULL && strcmp(lex_str[*index], "|") ; (*index)++, ++i) {
		if (strcmp(lex_str[*index], "<") == 0) {
			(*com)->redirect_in_path = (char*)lex_str[++(*index)];
		} else if (strcmp(lex_str[*index], ">") == 0) {
			(*com)->redirect_out_path = (char*)lex_str[++(*index)];
		} else if (strcmp(lex_str[*index], "&") == 0) {
			*isBkg = true;
		} else {
			(*com)->command_args[i] = (char*)lex_str[*index];
		}
	}

	(*com)->command_args[i] = NULL;
	(*com)->command_args[i+1] = NULL;

	if (lex_str[*index] == NULL) {
		return false;
	} else {
		(*index)++;
		return true;
	}
}

bool isLexValid(char *lex_str[MAX_LINE_LENGTH]) {
	for (char** ptr = (char**)lex_str; *ptr != NULL; ++ptr) {
		if (*ptr != NULL && strcmp(*ptr,"&") == 0) {
			while(*(ptr + 1) != NULL && **(ptr+1) == ' ') {
				ptr++;
			}
			if (*(ptr+1) != NULL && strcmp(*(ptr), "") != 0) {
				return false;
			}
		}

		if (*ptr != NULL && (strcmp(*ptr,"<") == 0 || strcmp(*ptr,">") == 0)) {
			while(*(ptr + 1) != NULL && **(ptr+1) == ' ') {
				ptr++;
			}
			if (*(++ptr) != NULL && isSymbol(**ptr)) {
				return false;
			}
		}
	}

	return true;
}

bool isCommValid(struct pipeline_command *comm) {
	bool notFirstCommand = false;
	for (struct pipeline_command* pPt = comm; pPt != NULL; pPt = pPt->next) {
		if (pPt->redirect_out_path != NULL && pPt->next != NULL) {
			return false;
		}
		if (notFirstCommand && pPt->redirect_in_path != NULL) {
			return false;
		}
		notFirstCommand = true;
	}

	return true;
}

void pipeline_command_free(struct pipeline_command *comm) {
	if(comm->next == NULL) {
		for (int i = 0; comm->command_args[i] != NULL; ++i) {
			free(comm->command_args[i]);
		}
		free(comm);
	} else {
		pipeline_command_free(comm->next);
		for (int i = 0; comm->command_args[i] != NULL; ++i) {
			free(comm->command_args[i]);
		}
		free(comm);
	}
}

struct pipeline *pipeline_build(const char *command_line)
{
	char *lex_str[MAX_LINE_LENGTH];
	memset(lex_str, 0, MAX_LINE_LENGTH * sizeof(char *));
	const char *it = command_line;
	int counter = 0;
	bool isBkg = false;

	/* tokenization process */
	while (*it != '\n' && *it != '\0') {
		lex_str[counter] = (char *) malloc(MAX_LINE_LENGTH * sizeof(char *));
		memset(lex_str[counter], 0, MAX_LINE_LENGTH * sizeof(char *));
		strcpy(lex_str[counter++], read_next_token(&it));
	}

	if (!isLexValid(lex_str)){
		return NULL;
	}

	/* write lexed string into pipeline_command */
	struct pipeline_command head;
	struct pipeline_command* prev = &head;
	bool end = true;
	int index = 0;

	while(end) {
		struct pipeline_command* curr = (struct pipeline_command*) malloc(sizeof(struct pipeline_command));
		memset(curr, 0, sizeof(struct pipeline_command));
		end = read_command(lex_str, &index, &curr, &isBkg);
		prev->next = curr;
		prev = prev->next;
	}

	static struct pipeline line;
	line.commands = head.next;
	line.is_background = isBkg;

	return ((isCommValid(line.commands)) ? &line : NULL);
}

void pipeline_free(struct pipeline *pipeline)
{
	pipeline_command_free(pipeline->commands);
}