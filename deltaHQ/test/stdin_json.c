#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

#include <node.h>

void print_json_aux(json_t *element, int indent);

void node_print(struct node_class *node) {
    printf("id=%d\tposition=(%.2f, %.2f, %.2f)\n",
           node->id,
           node->position.c[0],
           node->position.c[1],
           node->position.c[2]);
}

const char *json_plural(int count) {
    return count == 1 ? "" : "s";
}

void print_json_indent(int indent) {
    int i;
    for (i = 0; i < indent; i++) {
        putchar(' ');
    }
}

void print_json_object(json_t *element, int indent) {
    size_t size;
    const char *key;
    json_t *value;

    print_json_indent(indent);
    size = json_object_size(element);

    printf("JSON Object of %ld pair%s:\n", size, json_plural(size));
    json_object_foreach(element, key, value) {
        print_json_indent(indent + 2);
        printf("JSON Key: \"%s\"\n", key);
        print_json_aux(value, indent + 2);
    }
}

void print_json_array(json_t *element, int indent) {
    size_t i;
    size_t size = json_array_size(element);
    print_json_indent(indent);

    printf("JSON Array of %ld element%s:\n", size, json_plural(size));
    for (i = 0; i < size; i++) {
        print_json_aux(json_array_get(element, i), indent + 2);
    }
}

void print_json_string(json_t *element, int indent) {
    print_json_indent(indent);
    printf("JSON String: \"%s\"\n", json_string_value(element));
}

void print_json_integer(json_t *element, int indent) {
    print_json_indent(indent);
    printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n",
           json_integer_value(element));
}

void print_json_real(json_t *element, int indent) {
    print_json_indent(indent);
    printf("JSON Real: %f\n", json_real_value(element));
}

void print_json_true(json_t *element, int indent) {
    (void) element;
    print_json_indent(indent);
    printf("JSON True\n");
}

void print_json_false(json_t *element, int indent) {
    (void) element;
    print_json_indent(indent);
    printf("JSON False\n");
}

void print_json_null(json_t *element, int indent) {
    (void) element;
    print_json_indent(indent);
    printf("JSON Null\n");
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
    print_json_aux(root, 0);
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

int main(int argc, char *argv[]) {
    char line[4096];

    if (argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        exit(-1);
    }

    struct node_class c;
    struct node_class n;

    json_t *neighbors_array;
    json_t *neighbors;

    json_t *neighbor_array;
    json_t *neighbor;

    json_t *center_object;
    json_t *neighbor_object;

    json_t *node_object;

    int neighbors_array_len;
    int neighbor_array_len;

    while (read_line(line, 4096) != (char *) NULL) {

        printf("%s", line);
        json_t *root = load_json(line);

        if (root) {
            neighbors           = json_object_get(root, "neighbors");
            neighbors_array_len = json_array_size(neighbors);

            for (int i = 0; i < neighbors_array_len; i++) {
                neighbors_array = json_array_get(neighbors, i);

                center_object   = json_array_get(neighbors_array, 0);
                neighbor_object = json_array_get(neighbors_array, 1);

                node_object = json_object_get(center_object, "center");

                c.id = json_integer_value(json_object_get(node_object, "id"));
                c.position.c[0] =
                    json_integer_value(json_object_get(node_object, "x"));
                c.position.c[1] =
                    json_integer_value(json_object_get(node_object, "y"));

                printf("center: ");
                node_print(&c);

                neighbor_array = json_object_get(neighbor_object, "neighbor");
                neighbor_array_len = json_array_size(neighbor_array);

                // printf("neighbor len %d\n", neighbor_array_len);
                printf("neighbor: \n");
                for (int j = 0; j < neighbor_array_len; j++) {
                    neighbor = json_array_get(neighbor_array, j);
                    n.id =
                        json_integer_value(json_object_get(neighbor, "id"));
                    n.position.c[0] =
                        json_integer_value(json_object_get(neighbor, "x"));
                    n.position.c[1] =
                        json_integer_value(json_object_get(neighbor, "y"));
                    node_print(&n);
                }
            }
        }

        json_decref(root);
    }

    return 0;
}
