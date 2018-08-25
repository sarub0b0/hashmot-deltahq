#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

void print_json_aux(json_t *element, int indent);

const char *json_plural(int count) {
    return count == 1 ? "" : "s";
}

void print_json_indent(int indent) {
    int i;
    for (i = 0; i < indent; i++) {
        // putchar('');
    }
}

void print_json_object(json_t *element, int indent) {
    size_t size;
    const char *key;
    json_t *value;

    print_json_indent(indent);
    size = json_object_size(element);

    // printf("JSON Object of %ld pair%s:\n", size, json_plural(size));
    printf("{");
    json_object_foreach(element, key, value) {
        print_json_indent(indent);
        printf("\"%s\":", key);
        print_json_aux(value, indent);
        printf(",");
    }
    printf("\b},");
}

void print_json_array(json_t *element, int indent) {
    size_t i;
    size_t size = json_array_size(element);
    print_json_indent(indent);

    printf("[");
    // printf("JSON Array of %ld element%s:\n", size, json_plural(size));
    for (i = 0; i < size; i++) {
        print_json_aux(json_array_get(element, i), indent);
    }
    printf("\b");
    printf("],");
}

void print_json_string(json_t *element, int indent) {
    print_json_indent(indent);
    printf("\"%s\"", json_string_value(element));
}

void print_json_integer(json_t *element, int indent) {
    print_json_indent(indent);
    printf("\"%" JSON_INTEGER_FORMAT "\"", json_integer_value(element));
}

void print_json_real(json_t *element, int indent) {
    print_json_indent(indent);
    printf("%f\n", json_real_value(element));
}

void print_json_true(json_t *element, int indent) {
    (void) element;
    print_json_indent(indent);
    printf("True\n");
}

void print_json_false(json_t *element, int indent) {
    (void) element;
    print_json_indent(indent);
    printf("False\n");
}

void print_json_null(json_t *element, int indent) {
    (void) element;
    print_json_indent(indent);
    printf("Null\n");
}

void print_json_aux(json_t *element, int indent) {
    switch (json_typeof(element)) {
        case JSON_OBJECT:
            print_json_object(element, indent);
            break;
        case JSON_ARRAY:
            print_json_array(element, indent);
            break;
        case JSON_STRING:
            print_json_string(element, indent);
            break;
        case JSON_INTEGER:
            print_json_integer(element, indent);
            break;
        case JSON_REAL:
            print_json_real(element, indent);
            break;
        case JSON_TRUE:
            print_json_true(element, indent);
            break;
        case JSON_FALSE:
            print_json_false(element, indent);
            break;
        case JSON_NULL:
            print_json_null(element, indent);
            break;
        default:
            fprintf(
                stderr, "unrecognized JSON type %d\n", json_typeof(element));
    }
}

void print_json(json_t *root) {
    // printf("{");
    print_json_aux(root, 0);
    printf("\b");
    printf("\n");
}

json_t *load_json(const char *text) {
    json_t *root;
    json_error_t error;

    root = json_loads(text, 0, &error);

    if (root) {
        return root;
    } else {
        fprintf(
            stderr, "json error on line %d: %s\n", error.line, error.text);
        return (json_t *) 0;
    }
}

char *read_line(char *line, int max_chars) {
    printf("Type some JSON > ");
    fflush(stdout);
    return fgets(line, max_chars, stdin);
}

// int motion_query(json_t *motion, int motion_number) {
//     printf("{\'motion\':[");
//     for (int i = 0; i < motion_number; i++) {
//         // printf("{\'id\':%d,\'x\':%d,\'y\':%d},",motion[i].node_index, motion[i].coo)
//     }
// }

int main(void) {

    json_t *motion;
    json_t *node_array;
    json_t **node;
    json_t **x, **y;
    json_t **coordinate;
    json_t **id;

    int motion_number = 3;

    // char id[BUFSIZ];

    node       = (json_t **) malloc(sizeof(json_t *) * motion_number);
    coordinate = (json_t **) malloc(sizeof(json_t *) * motion_number);
    x          = (json_t **) malloc(sizeof(json_t *) * motion_number);
    y          = (json_t **) malloc(sizeof(json_t *) * motion_number);
    id         = (json_t **) malloc(sizeof(json_t *) * motion_number);

    node_array = json_array();

    for (int i = 0; i < motion_number; i++) {
        // node[i]       = json_object();
        node[i]       = json_array();
        coordinate[i] = json_array();

        id[i] = json_object();
        x[i]  = json_object();
        y[i]  = json_object();

        json_object_set(x[i], "x", json_integer((json_int_t) i));
        json_object_set(y[i], "y", json_integer((json_int_t) i));
        json_object_set(id[i], "id", json_integer((json_int_t) i));

        // json_array_append(coordinate[i], x[i]);
        // json_array_append(coordinate[i], y[i]);
        json_array_append(node[i], x[i]);
        json_array_append(node[i], y[i]);
        json_array_append(node[i], id[i]);

        // snprintf(id, BUFSIZ, "%d", i);
        // json_object_set(node[i], id, coordinate[i]);

        json_array_append(node_array, node[i]);
    }

    motion = json_object();
    json_object_set(motion, "motion", node_array);

    // motion_query(node_array, motion_number);
    // print_json(motion);

    return 0;
}
