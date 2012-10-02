
#include "ast.h"
#include "front_end_params.h"
#include "smt_context.h"
#include "arith_decl_plugin.h"
#include "bv_decl_plugin.h"
#include "array_decl_plugin.h"
#include "model_v2_pp.h"

void tst_model_retrieval()
{          
    memory::initialize(0);
    front_end_params params;
    params.m_model = true;


    ast_manager m;
    m.register_decl_plugins();

    family_id array_fid = m.get_family_id(symbol("array"));
    array_util au(m);
    array_decl_plugin& ad = *static_cast<array_decl_plugin *>(m.get_plugin(array_fid));


    // arr_s and select_fn creation copy-pasted from z3.cpp

    parameter sparams[2]  = { parameter(to_sort(m.mk_bool_sort())), parameter(to_sort(m.mk_bool_sort())) };
    sort_ref arr_s(m.mk_sort(array_fid, ARRAY_SORT, 2, sparams), m);

    sort * domain2[2] = {arr_s, m.mk_bool_sort()};
    func_decl_ref select_fn(
        m.mk_func_decl(array_fid, OP_SELECT, 2, arr_s->get_parameters(), 2, domain2), m);


    app_ref a1(m.mk_const(symbol("a1"), arr_s), m);
    app_ref a2(m.mk_const(symbol("a2"), arr_s), m);

    // (= true (select a1 true))
    app_ref fml(m.mk_eq(m.mk_true(),
        m.mk_app(select_fn.get(), a1, m.mk_true())), m);

    smt::context ctx(m, params);
    ctx.assert_expr(fml);
    lbool check_result = ctx.check();
    std::cout<<((check_result==l_true) ? "satisfiable" : 
                (check_result==l_false) ? "unsatisfiable" : "unknown")<<"\n";
    ref<model> model;
    ctx.get_model(model);
    model_v2_pp(std::cout, *model, false);
    expr_ref a1_val(model->get_const_interp(a1->get_decl()), m);

    app_ref fml2(m.mk_eq(a2, a1_val), m);
    ctx.assert_expr(fml2);
    std::cout<<"--------------------------\n";
    ctx.display(std::cout);
    std::cout<<"--------------------------\n";
    check_result = ctx.check();
    ctx.display(std::cout);
    std::cout<<"--------------------------\n";
    std::cout<<((check_result==l_true) ? "satisfiable" : 
                (check_result==l_false) ? "unsatisfiable" : "unknown")<<"\n";
}
