#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

#define KERNEL_TEST_ENABLED
#include "../kernels/vm.cl"

int tests_run = 0;

/* Helper Functions. */
packet *q_create() {
  packet *q = malloc((16 + (QUEUE_SIZE * 16)) * sizeof(packet));
  if (q) {
    for (int i = 0; i < (16 + (QUEUE_SIZE * 16)); i++) {
      q[i].x = 0;
      q[i].y = 0;
    }
  }

  return q;
}

void q_destroy(packet *q) {
  free(q);
}

subt_rec *subt_rec_create(uint service_id) {
  subt_rec *rec = malloc(sizeof(subt_rec));
  if (rec) {
    rec->service_id = service_id;
  }

  return rec;
}

void subt_rec_destroy(subt_rec *rec) {
  free(rec);
}

subt *subt_create() {
  subt *subt = malloc(sizeof(subt));
  if (subt) {
    subt->av_recs[0] = 0;
    for (int i = 0; i < SUBT_SIZE; i++) {
      subt->av_recs[i + 1] = i;
    }
  }

  return subt;
}

void subt_destroy(subt *subt) {
  free(subt);
}

/* Unit Tests */
static char *test_pkt_get_type() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  mu_assert("FAIL: test_pkt_get_type [1]", pkt_get_type(p) == REFERENCE);
  mu_assert("FAIL: test_pkt_get_type [2]", pkt_get_type(p) != ERROR);
  return NULL;
}

static char *test_pkt_get_dest() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  mu_assert("FAIL: test_pkt_get_dest [1]", pkt_get_dest(p) != 2);
  mu_assert("FAIL: test_pkt_get_dest [2]", pkt_get_dest(p) == 1);
  return NULL;
}

static char *test_pkt_get_arg_pos() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  mu_assert("FAIL: test_pkt_get_arg_pos [1]", pkt_get_arg_pos(p) != 3);
  mu_assert("FAIL: test_pkt_get_arg_pos [2]", pkt_get_arg_pos(p) == 2);
  return NULL;
}

static char *test_pkt_get_sub() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  mu_assert("FAIL: test_pkt_get_sub [1]", pkt_get_sub(p) != 4);
  mu_assert("FAIL: test_pkt_get_sub [2]", pkt_get_sub(p) == 3);
  return NULL;
}

static char *test_pkt_get_payload() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  mu_assert("FAIL: test_pkt_get_payload [1]", pkt_get_payload(p) != 1);
  mu_assert("FAIL: test_pkt_get_payload [2]", pkt_get_payload(p) == 4);
  return NULL;
}

static char *test_pkt_set_type() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  pkt_set_type(&p, ERROR);
  mu_assert("FAIL: test_pkt_set_type [1]", pkt_get_type(p) != REFERENCE);
  mu_assert("FAIL: test_pkt_set_type [2]", pkt_get_type(p) == ERROR);
  return NULL;
}

static char *test_pkt_set_dest() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  pkt_set_dest(&p, 7);
  mu_assert("FAIL: test_pkt_set_dest [1]", pkt_get_dest(p) != 2);
  mu_assert("FAIL: test_pkt_set_dest [2]", pkt_get_dest(p) == 7);
  return NULL;
}

static char *test_pkt_set_arg_pos() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  pkt_set_arg_pos(&p, 7);
  mu_assert("FAIL: test_pkt_set_arg_pos [1]", pkt_get_arg_pos(p) != 3);
  mu_assert("FAIL: test_pkt_set_arg_pos [2]", pkt_get_arg_pos(p) == 7);
  return NULL;
}

static char *test_pkt_set_sub() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  pkt_set_sub(&p, 7);
  mu_assert("FAIL: test_pkt_set_sub [1]", pkt_get_sub(p) != 4);
  mu_assert("FAIL: test_pkt_set_sub [2]", pkt_get_sub(p) == 7);
  return NULL;
}

static char *test_pkt_set_payload() {
  packet p = pkt_create(REFERENCE, 1, 2, 3, 4);
  pkt_set_payload(&p, 7);
  mu_assert("FAIL: test_pkt_set_payload [1]", pkt_get_payload(p) != 1);
  mu_assert("FAIL: test_pkt_set_payload [2]", pkt_get_payload(p) == 7);
  return NULL;
}

static char *test_pkt_create() {
  packet p = pkt_create(ERROR, 1, 2, 3, 4);
  mu_assert("FAIL: test_pkt_create [1]", pkt_get_type(p) == ERROR);
  mu_assert("FAIL: test_pkt_create [2]", pkt_get_dest(p) == 1);
  mu_assert("FAIL: test_pkt_create [3]", pkt_get_arg_pos(p) == 2);
  mu_assert("FAIL: test_pkt_create [4]", pkt_get_sub(p) == 3);
  mu_assert("FAIL: test_pkt_create [5]", pkt_get_payload(p) == 4);
  return NULL;
}

static char *test_q_get_head_index() {
  packet *q = q_create();
  packet i;
  mu_assert("FAIL: test_q_get_head_index [1]", q_get_head_index(0, 0, q, 4) == 0);
  q_write(i, 0, q, 4);
  q_read(&i, 0, q, 4);
  mu_assert("FAIL: test_q_get_head_index [2]", q_get_head_index(0, 0, q, 4) == 1);
  q_destroy(q);
  return NULL;
}

static char *test_q_get_tail_index() {
  packet *q = q_create();
  packet i;
  mu_assert("FAIL: test_q_get_tail_index [1]", q_get_tail_index(0, 0, q, 4) == 0);
  q_write(i, 1, q, 4);
  q_write(i, 1, q, 4);
  mu_assert("FAIL: test_q_get_tail_index [2]", q_get_tail_index(1, 0, q, 4) == 2);
  mu_assert("FAIL: test_q_get_tail_index [3]", q_get_tail_index(0, 0, q, 4) == 0);
  q_write(i, 0, q, 4);
  mu_assert("FAIL: test_q_get_tail_index [4]", q_get_tail_index(0, 0, q, 4) == 1);
  q_destroy(q);
  return NULL;
}

static char *test_q_set_head_index() {
  packet *q = q_create();
  q_set_head_index(10, 0, 0, q, 4);
  mu_assert("FAIL: test_q_set_head_index [1]", q_get_head_index(0, 0, q, 4) == 10);
  q_set_head_index(0, 0, 0, q, 4);
  mu_assert("FAIL: test_q_set_head_index [2]", q_get_head_index(0, 0, q, 4) == 0);
  mu_assert("FAIL: test_q_set_head_index [3]", q_get_head_index(1, 0, q, 4) == 0);
  q_destroy(q);
  return NULL;
}

static char *test_q_set_tail_index() {
  packet *q = q_create();
  q_set_tail_index(10, 0, 0, q, 4);
  mu_assert("FAIL: test_q_set_tail_index [1]", q_get_tail_index(0, 0, q, 4) == 10);
  q_set_tail_index(0, 0, 0, q, 4);
  mu_assert("FAIL: test_q_set_tail_index [2]", q_get_tail_index(0, 0, q, 4) == 0);
  mu_assert("FAIL: test_q_set_tail_index [3]", q_get_tail_index(1, 0, q, 4) == 0);
  q_destroy(q);
  return NULL;
}

static char *test_q_set_last_op() {
  packet *q = q_create();
  q_set_last_op(WRITE, 0, 0, q, 4);
  mu_assert("FAIL: test_q_set_last_op [1]", q_last_op_is_write(0, 0, q, 4));
  q_set_last_op(READ, 4, 0, q, 4);
  mu_assert("FAIL: test_q_set_last_op [2]", !q_last_op_is_write(4, 0, q, 4));
  mu_assert("FAIL: test_q_set_last_op [3]", q_last_op_is_read(4, 0, q, 4));
  q_destroy(q);
  return NULL;
}

static char *test_q_last_op_is_read() {
  packet *q = q_create();
  q_set_last_op(READ, 0, 0, q, 4);
  mu_assert("FAIL: test_q_last_op_is_read [1]", q_last_op_is_read(0, 0, q, 4));
  q_set_last_op(READ, 1, 0, q, 4);
  mu_assert("FAIL: test_q_last_op_is_read [2]", q_last_op_is_read(1, 0, q, 4));
  q_set_last_op(READ, 2, 0, q, 4);
  mu_assert("FAIL: test_q_last_op_is_read [3]", q_last_op_is_read(2, 0, q, 4));
  q_set_last_op(WRITE, 3, 0, q, 4);
  mu_assert("FAIL: test_q_last_op_is_read [4]", !q_last_op_is_read(3, 0, q, 4));
  q_destroy(q);
  return NULL;
}

static char *test_q_last_op_is_write() {
  packet *q = q_create();
  q_set_last_op(WRITE, 0, 0, q, 4);
  mu_assert("FAIL: test_q_last_op_is_write [1]", q_last_op_is_write(0, 0, q, 4));
  q_set_last_op(WRITE, 1, 0, q, 4);
  mu_assert("FAIL: test_q_last_op_is_write [2]", q_last_op_is_write(1, 0, q, 4));
  q_set_last_op(WRITE, 2, 0, q, 4);
  mu_assert("FAIL: test_q_last_op_is_write [3]", q_last_op_is_write(2, 0, q, 4));
  q_set_last_op(READ, 3, 0, q, 4);
  mu_assert("FAIL: test_q_last_op_is_write [4]", !q_last_op_is_write(3, 0, q, 4));
  q_destroy(q);
  return NULL;
}

static char *test_q_is_empty() {
  packet *q = q_create();
  packet p = pkt_create(ERROR, 0, 0, 0, 0);
  mu_assert("FAIL: test_q_is_empty [1]", q_is_empty(0, 0, q, 4));
  q_write(p, 0, q, 4);
  mu_assert("FAIL: test_q_is_empty [2]", !q_is_empty(0, 0, q, 4));
  q_read(&p, 0, q, 4);
  mu_assert("FAIL: test_q_is_empty [3]", q_is_empty(0, 0, q, 4));
  q_destroy(q);
  return NULL;
}

static char *test_q_is_full() {
  packet *q = q_create();
  packet p = pkt_create(ERROR, 0, 0, 0, 0);
  mu_assert("FAIL: test_q_is_full [1]", !q_is_full(0, 0, q, 4));
  for (int i = 0; i < 16; i++) {
    q_write(p, 0, q, 4);
  }
  mu_assert("FAIL: test_q_is_full [2]", q_is_full(0, 0, q, 4));
  q_read(&p, 0, q, 4);
  mu_assert("FAIL: test_q_is_full [3]", !q_is_full(0, 0, q, 4));
  q_destroy(q);
  return NULL;
}

static char *test_q_size() {
  packet *q = q_create();
  packet p = pkt_create(ERROR, 0, 0, 0, 0);
  mu_assert("FAIL: test_q_size [1]", q_size(0, 0, q, 4) == 0);
  for (int i = 0; i < 20; i++) {
    q_write(p, 0, q, 4);
  }
  mu_assert("FAIL: test_q_size [2]", q_size(0, 0, q, 4) == 16);
  q_read(&p, 0, q, 4);
  mu_assert("FAIL: test_q_size [3]", q_size(0, 0, q, 4) == 15);
  q_destroy(q);
  return NULL;
}

static char *test_q_read() {
  packet *q = q_create();
  packet p = pkt_create(REFERENCE, 0, 0, 0, 0);
  packet p2 = pkt_create(ERROR, 0, 0, 0, 0);
  q_write(p, 0, q, 4);
  q_read(&p2, 0, q, 4);
  mu_assert("FAIL: test_q_read [1]", (p.x == p2.x) && (p.y == p2.y));
  q_read(&p2, 0, q, 4);
  mu_assert("FAIL: test_q_read [2]", !q_read(&p2, 0, q, 4));
  q_destroy(q);
  return NULL;
}

static char *test_q_write() {
  packet *q = q_create();
  packet p = pkt_create(DATA, 1, 2, 3, 4);
  q_write(p, 0, q, 4);
  mu_assert("FAIL: test_q_write [1]", !q_is_empty(0, 0, q, 4));
  q_write(p, 0, q, 4);
  mu_assert("FAIL: test_q_write [2]", q_size(0, 0, q, 4) == 2);
  for (int i = 0; i < 14; i++) {
    q_write(p, 0, q, 4);
  }
  mu_assert("FAIL: test_q_write [3]", !q_write(p, 0, q, 4));
  q_destroy(q);
  return NULL;
}

static char *test_cunit_q_is_empty() {
  packet *q = q_create();
  packet p = pkt_create(DATA, 1, 2, 3, 4);
  mu_assert("FAIL: test_cunit_is_empty [1]", cunit_q_is_empty(0, q, 4));
  q_write(p, 0, q, 4);
  mu_assert("FAIL: test_cunit_is_empty [2]", !cunit_q_is_empty(0, q, 4));
  mu_assert("FAIL: test_cunit_is_empty [3]", cunit_q_is_empty(1, q, 4));
  mu_assert("FAIL: test_cunit_is_empty [4]", cunit_q_is_empty(2, q, 4));
  mu_assert("FAIL: test_cunit_is_empty [5]", cunit_q_is_empty(3, q, 4));
  q_destroy(q);
  return NULL;
}

static char *test_cunit_q_is_full() {
  packet *q = q_create();
  mu_assert("FAIL: test_cunit_is_full [1]", !cunit_q_is_full(0, q, 4));
  mu_assert("FAIL: test_cunit_is_full [3]", !cunit_q_is_full(1, q, 4));
  mu_assert("FAIL: test_cunit_is_full [4]", !cunit_q_is_full(2, q, 4));
  mu_assert("FAIL: test_cunit_is_full [5]", !cunit_q_is_full(3, q, 4));

  /* Can't simply test for cunit fullness - Forced to hack. */
  packet p;
  p.x = 0;
  p.y = WRITE;
  q[0] = q[1] = q[2] = q[3] = p;
  mu_assert("FAIL: test_cunit_is_full [6]", cunit_q_is_full(0, q, 4));
  q_destroy(q);
  return NULL;
}

static char *test_cunit_q_size() {
  packet *q = q_create();
  packet p = pkt_create(DATA, 1, 2, 3, 4);
  mu_assert("FAIL: test_cunit_q_size", cunit_q_size(0, q, 4) == 0);
  q_write(p, 0, q, 4);
  mu_assert("FAIL: test_cunit_q_size", cunit_q_size(0, q, 4) == 1);
  for (int i = 0; i < 20; i++) {
    q_write(p, 0, q, 4);
  }
  mu_assert("FAIL: test_cunit_q_size", cunit_q_size(0, q, 4) == 16);
  q_destroy(q);
  return NULL;
}

static char *test_transferRQ() {
  packet *q = q_create();
  packet *rq = q_create();
  packet p = pkt_create(DATA, 1, 2, 3, 4);
  packet p2;
  q_write(p, 0, rq, 4);
  transferRQ(rq, q, 4);
  q_read(&p2, 0, q, 4);
  mu_assert("FAIL: test_transferRQ", (p.x == p2.x) && (p.y == p2.y));
  q_destroy(q);
  q_destroy(rq);
  return NULL;
}

static char *test_subt_rec_get_service_id() {
  subt_rec *rec = subt_rec_create(1);
  mu_assert("FAIL: test_subt_rec_get_service_id", subt_rec_get_service_id(*rec) == 1);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_get_arg() {
  subt_rec *rec = subt_rec_create(1);
  mu_assert("FAIL: test_subt_rec_get_arg", subt_rec_get_arg(*rec, 0) == 0);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_get_arg_mode() {
  subt_rec *rec = subt_rec_create(1);
  mu_assert("FAIL: test_subt_rec_get_arg_mode", subt_rec_get_arg_mode(*rec, 2) == 0);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_get_subt_status() {
  subt_rec *rec = subt_rec_create(1);
  mu_assert("FAIL: test_subt_rec_get_subt_status", subt_rec_get_subt_status(*rec) == 0);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_get_nargs_absent() {
  subt_rec *rec = subt_rec_create(1);
  mu_assert("FAIL: test_subt_rec_get_nargs_absent", subt_rec_get_nargs_absent(*rec) == 0);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_get_return_to() {
  subt_rec *rec = subt_rec_create(1);
  mu_assert("FAIL: test_subt_rec_get_return_to", subt_rec_get_return_to(*rec) == 0);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_get_return_as() {
  subt_rec *rec = subt_rec_create(1);
  mu_assert("FAIL: test_subt_rec_get_return_as", subt_rec_get_return_as(*rec) == 0);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_set_service_id() {
  subt_rec *rec = subt_rec_create(1);
  subt_rec_set_service_id(rec, 2);
  mu_assert("FAIL: test_subt_rec_set_service_id", subt_rec_get_service_id(*rec) == 2);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_set_arg() {
  subt_rec *rec = subt_rec_create(1);
  subt_rec_set_arg(rec, 0, 50);
  mu_assert("FAIL: test_subt_rec_set_arg", subt_rec_get_arg(*rec, 0) == 50);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_set_arg_mode() {
  subt_rec *rec = subt_rec_create(1);
  subt_rec_set_arg_mode(rec, 0, BY_REF);
  mu_assert("FAIL: test_subt_rec_set_arg_mode", subt_rec_get_arg_mode(*rec, 0) == BY_REF);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_set_subt_status() {
  subt_rec *rec = subt_rec_create(1);
  subt_rec_set_subt_status(rec, PROCESSING);
  mu_assert("FAIL: test_subt_rec_set_subt_status", subt_rec_get_subt_status(*rec) == PROCESSING);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_set_nargs_absent() {
  subt_rec *rec = subt_rec_create(1);
  subt_rec_set_nargs_absent(rec, 4);
  mu_assert("FAIL: test_subt_rec_set_nargs_absent", subt_rec_get_nargs_absent(*rec) == 4);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_set_return_to() {
  subt_rec *rec = subt_rec_create(1);
  subt_rec_set_return_to(rec, 100);
  mu_assert("FAIL: test_subt_rec_set_return_to", subt_rec_get_return_to(*rec) == 100);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_rec_set_return_as() {
  subt_rec *rec = subt_rec_create(1);
  subt_rec_set_return_as(rec, 255);
  mu_assert("FAIL: test_subt_rec_set_return_as", subt_rec_get_return_as(*rec) == 255);
  subt_rec_destroy(rec);
  return NULL;
}

static char *test_subt_is_full() {
  subt *subt = subt_create();
  mu_assert("FAIL: test_subt_is_full", !subt_is_full(subt));
  subt_destroy(subt);
  return NULL;
}

static char *test_subt_is_empty() {
  subt *subt = subt_create();
  mu_assert("FAIL: test_subt_is_empty", subt_is_empty(subt));
  subt_destroy(subt);
  return NULL;
}

static char *test_subt_top() {
  subt *subt = subt_create();
  mu_assert("FAIL: test_subt_top", subt_top(subt) == 0);
  subt_destroy(subt);
  return NULL;
}

static char *test_subt_set_top() {
  subt *subt = subt_create();
  subt_set_top(subt, 7);
  mu_assert("FAIL: test_subt_set_top", subt_top(subt) == 7);
  subt_destroy(subt);
  return NULL;
}

static char *test_subt_push() {
  subt *subt = subt_create();
  mu_assert("FAIL: test_subt_push [1]", !subt_push(0, subt));
  ushort i;
  subt_pop(&i, subt);
  mu_assert("FAIL: test_subt_push [2]", subt_push(0, subt));
  mu_assert("FAIL: test_subt_push [3]", !subt_push(1, subt));
  subt_destroy(subt);
  return NULL;
}

static char *test_subt_pop() {
  subt *subt = subt_create();
  ushort i = 0;
  mu_assert("FAIL: test_subt_pop [1]", subt_pop(&i, subt));
  mu_assert("FAIL: test_subt_pop [2]", subt_pop(&i, subt));
  mu_assert("FAIL: test_subt_pop [3]", i == 1);
  subt_destroy(subt);
  return NULL;
}

static char *test_subt_get_rec() {
  subt *subt = subt_create();
  subt_rec *rec;
  for (int i = 0; i < SUBT_SIZE; i++) {
    mu_assert("FAIL: test_subt_get_rec", subt_get_rec(rec, subt));
  }
  subt_destroy(subt);
  return NULL;
}

static char *all_tests() {
  mu_run_test(test_pkt_get_type);
  mu_run_test(test_pkt_get_dest);
  mu_run_test(test_pkt_get_arg_pos);
  mu_run_test(test_pkt_get_sub);
  mu_run_test(test_pkt_get_payload);
  mu_run_test(test_pkt_set_type);
  mu_run_test(test_pkt_set_dest);
  mu_run_test(test_pkt_set_arg_pos);
  mu_run_test(test_pkt_set_sub);
  mu_run_test(test_pkt_set_payload);
  mu_run_test(test_pkt_create);

  mu_run_test(test_q_get_head_index);
  mu_run_test(test_q_get_tail_index);
  mu_run_test(test_q_set_head_index);
  mu_run_test(test_q_set_tail_index);
  mu_run_test(test_q_set_last_op);
  mu_run_test(test_q_last_op_is_read);
  mu_run_test(test_q_last_op_is_write);
  mu_run_test(test_q_is_empty);
  mu_run_test(test_q_is_full);
  mu_run_test(test_q_size);
  mu_run_test(test_q_read);
  mu_run_test(test_q_write);

  mu_run_test(test_cunit_q_is_empty);
  mu_run_test(test_cunit_q_is_full);
  mu_run_test(test_cunit_q_size);
  mu_run_test(test_transferRQ);

  mu_run_test(test_subt_rec_get_service_id);
  mu_run_test(test_subt_rec_get_arg);
  mu_run_test(test_subt_rec_get_arg_mode);
  mu_run_test(test_subt_rec_get_subt_status);
  mu_run_test(test_subt_rec_get_nargs_absent);
  mu_run_test(test_subt_rec_get_return_to);
  mu_run_test(test_subt_rec_get_return_as);
  mu_run_test(test_subt_rec_set_service_id);
  mu_run_test(test_subt_rec_set_arg);
  mu_run_test(test_subt_rec_set_arg_mode);
  mu_run_test(test_subt_rec_set_subt_status);
  mu_run_test(test_subt_rec_set_nargs_absent);
  mu_run_test(test_subt_rec_set_return_to);
  mu_run_test(test_subt_rec_set_return_as);

  mu_run_test(test_subt_is_full);
  mu_run_test(test_subt_is_empty);
  mu_run_test(test_subt_top);
  mu_run_test(test_subt_set_top);
  mu_run_test(test_subt_push);
  mu_run_test(test_subt_pop);
  mu_run_test(test_subt_get_rec);

  return NULL;
}

int main(void) {
  char *result = all_tests();
  if (result) {
    printf("%s\n", result);
  }
  else {
    printf("ALL TESTS PASSED\n");
  }

  printf("Tests run: %d\n", tests_run);
  return result != 0;
}
