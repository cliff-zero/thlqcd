#pragma once

#include "data-load.h"
#include "baryon-pion-block-acc.h"
#include "baryon-pion-coefficient.h"
#include "compute-utils.h"

namespace qlat
{

    inline std::string get_baryon_pi_rescattering_all_psel_arbitrarty_func_psel_path(const std::string &job_tag,
                                                                                     const int &traj)
    {
        return ssprintf("analysis/baryon_pi_rescattering_fsel_mate_ultra_magic_pro/%s/results=%d", job_tag.c_str(),
                        traj);
    }

    inline LatData mk_baryon_pi_rescattering_fsel_for_arbitrary_mate(
        const int &num_dtxy, const int &gram_num,
        const std::vector<std::vector<int>> &Momentum_Targets_src,
        const std::vector<std::vector<int>> &Momentum_Targets_curr,
        const Coordinate &total_site)
    {
        LatData ld;
        ld.info.push_back(lat_dim_number("txy", 0, num_dtxy - 1));
        ld.info.push_back(lat_dim_number("momentum_mode_src", 0, Momentum_Targets_src.size() - 1));
        ld.info.push_back(lat_dim_number("momentum_mode_curr", 0, Momentum_Targets_curr.size() - 1));
        ld.info.push_back(lat_dim_number("gram", 0, 2 * gram_num - 1));
        ld.info.push_back(lat_dim_number("VAmu", 0, 8 - 1));
        ld.info.push_back(lat_dim_number("mu", 0, 3));
        ld.info.push_back(lat_dim_number("nu", 0, 3));
        ld.info.push_back(lat_dim_re_im());
        lat_data_alloc(ld);
        set_zero(ld);
        return ld;
    }

    inline void contract_leading_tiwst_topology_fsel_mate_ultra(
        const std::string &job_tag, const int &traj,
        const std::vector<std::vector<Baryon_Block_No_Projection_Psel_Psel>> &block,
        const std::vector<std::vector<std::vector<std::vector<std::vector<Sequential_Prop_2pt_Block>>>>> &block_x_y,
        const std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<ComplexD>>>>>> &baryon_pi_two_func_coeff,
        const std::vector<std::vector<int>> &Momentum_Targets_src,
        const std::vector<std::vector<int>> &Momentum_Targets_curr,
        const bool &is_baryon_smear,
        const PointsSelection &psel_baryon,
        const long &psel_baryon_num,
        const std::vector<int> &psel_baryon_num_list,
        const std::vector<int> &list_n_from_idx_baryon,
        const std::vector<std::vector<long>> &idx_class_by_time_baryon,
        const bool &is_meson_smear,
        const PointsSelection &psel_meson,
        const long &psel_meson_num,
        const std::vector<int> &psel_meson_num_list,
        const std::vector<int> &list_n_from_idx_meson,
        const std::vector<std::vector<long>> &idx_class_by_time_meson,
        const std::vector<int> &fsel_num_list,
        const std::vector<int> &list_n_from_idx_fsel,
        const std::vector<std::vector<long>> &idx_class_by_time_fsel,
        const FieldSelection &fsel, const Geometry &geo,
        const int &ti, const int &dtmax, const int &dtmin,
        const int &t_baryon_snk, const int &xt, const int &t_meson, const int &t_baryon_src, const int &dtxy, const int &t_src_snk,
        std::vector<std::vector<ComplexD>> &leading_tiwst_result,
        const double &anti_period,
        const int &gram_num)
    {
        TIMER_VERBOSE("contract_leading_tiwst_topology_fsel_mate_ultra");
        const Coordinate &total_site = geo.total_site();

        const int &mom_num_src = Momentum_Targets_src.size();
        const int &mom_num_curr = Momentum_Targets_curr.size();

#pragma omp parallel for collapse(3)
        for (int idx_baryon_src = 0; idx_baryon_src < psel_baryon_num_list[t_baryon_src]; idx_baryon_src++)
        {
            for (int idx_baryon_snk = 0; idx_baryon_snk < psel_baryon_num_list[t_baryon_snk]; idx_baryon_snk++)
            {
                for (int idx_meson_ty = 0; idx_meson_ty < psel_meson_num_list[t_meson]; idx_meson_ty++)
                {
                    // if (0 == fsel_num_list[xt])
                    // {
                    //     continue;
                    // }

                    for (int mom_curr_id = 0; mom_curr_id < mom_num_curr; mom_curr_id++)
                    {
                        const long &xg_src_psel_idx = idx_class_by_time_baryon[t_baryon_src][idx_baryon_src];
                        const Coordinate &xg_src = psel_baryon[xg_src_psel_idx];

                        const long &xg_snk_psel_idx = idx_class_by_time_baryon[t_baryon_snk][idx_baryon_snk];
                        const Coordinate &xg_snk = psel_baryon[xg_snk_psel_idx];

                        const long &xg_y_psel_idx = idx_class_by_time_meson[t_meson][idx_meson_ty];
                        const Coordinate &xg_y = psel_meson[xg_y_psel_idx];

                        // long xg_x_fsel_idx = idx_class_by_time_fsel[xt][idx_x];
                        // const long index = fsel.indices[xg_x_fsel_idx];
                        // const Coordinate xg_x_l = geo.coordinate_from_index(index);
                        // const Coordinate xg_x_g = geo.coordinate_g_from_l(xg_x_l);
                        // const Coordinate &xg_x = xg_x_g;
                        // const int &t_x = xg_x[3];
                        // qassert(t_x == xt);
                        // qassert(idx_x == list_n_from_idx_fsel[xg_x_fsel_idx]);

                        if (is_baryon_smear == is_meson_smear && (xg_snk_psel_idx == xg_y_psel_idx || xg_src_psel_idx == xg_y_psel_idx))
                        {
                            continue;
                        }

                        qassert(list_n_from_idx_baryon[xg_src_psel_idx] == idx_baryon_src);
                        qassert(list_n_from_idx_baryon[xg_snk_psel_idx] == idx_baryon_snk);
                        qassert(list_n_from_idx_meson[xg_y_psel_idx] == idx_meson_ty);

                        const Baryon_Block_No_Projection_Psel_Psel &ppblock_np = block[idx_baryon_snk][idx_baryon_src];

                        for (int VA_idx = 0; VA_idx < 8; VA_idx++)
                        {
                            const Sequential_Prop_2pt_Block &block_x_y_temp = block_x_y[idx_baryon_snk][idx_baryon_src][idx_meson_ty][mom_curr_id][VA_idx];

                            qassert(ppblock_np.is_build);
                            // if (!block_x_y_temp.is_build)
                            // {
                            //     std::cout << block_x_y_temp.is_build << std::endl;
                            // }
                            qassert(block_x_y_temp.is_build);
                            qassert(block_x_y_temp.xg_snk_psel_idx == xg_snk_psel_idx);
                            qassert(block_x_y_temp.xg_src_psel_idx == xg_src_psel_idx);
                            // qassert(block_x_y_temp.xg_x_psel_idx == xg_x_fsel_idx);
                            qassert(block_x_y_temp.xg_y_psel_idx == xg_y_psel_idx);

                            const WilsonMatrix &wm_seq_snk_src = block_x_y_temp.block[0];
                            const WilsonMatrix &wm_seq_exch_snk_src = block_x_y_temp.block[1];

                            std::vector<SpinMatrix> spin_result_list_temp, spin_result_list_ex_temp;
                            contract_swm_ppblock_np(spin_result_list_temp, ppblock_np, wm_seq_snk_src);
                            contract_swm_ppblock_np(spin_result_list_ex_temp, ppblock_np, wm_seq_exch_snk_src);

                            for (int mom_src_id = 0; mom_src_id < mom_num_src; ++mom_src_id)
                            {
                                long phase_temp = space_dot(Momentum_Targets_src[mom_src_id], xg_src) - space_dot(Momentum_Targets_src[mom_src_id], xg_y) + space_dot(Momentum_Targets_curr[mom_curr_id], xg_snk);
                                int phase = mod(phase_temp, total_site[0]);

                                const ComplexD normal_coeff = baryon_pi_two_func_coeff[ti][t_src_snk][mom_src_id][mom_curr_id][dtxy][phase] * (ComplexD)anti_period;

                                for (int topology_id = 0; topology_id < 5; topology_id++)
                                {
                                    SpinMatrix spin_result_list = spin_result_list_temp[topology_id] * normal_coeff;
                                    // std::cout<<spin_result_list(0, 0)<<std::endl;
                                    for (int mu1 = 0; mu1 < 4; mu1++)
                                    {
                                        for (int mu2 = 0; mu2 < 4; mu2++)
                                        {
                                            if (spin_result_list(mu1, mu2) == 0.0)
                                            {
                                                continue;
                                            }
                                            leading_tiwst_result[omp_get_thread_num()][((((mom_src_id * mom_num_curr + mom_curr_id) * gram_num + topology_id) * 8 + VA_idx) * 4 + mu1) * 4 + mu2] += spin_result_list(mu1, mu2);
                                        }
                                    }

                                    SpinMatrix spin_result_list_ex = spin_result_list_ex_temp[topology_id] * normal_coeff;
                                    for (int mu1 = 0; mu1 < 4; mu1++)
                                    {
                                        for (int mu2 = 0; mu2 < 4; mu2++)
                                        {
                                            if (spin_result_list_ex(mu1, mu2) == 0.0)
                                            {
                                                continue;
                                            }
                                            leading_tiwst_result[omp_get_thread_num()][((((mom_src_id * mom_num_curr + mom_curr_id) * gram_num + topology_id + 5) * 8 + VA_idx) * 4 + mu1) * 4 + mu2] += spin_result_list_ex(mu1, mu2);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    inline void leading_tiwst_mate_fsel(
        const std::string &job_tag, const int &traj,
        const std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<ComplexD>>>>>> &baryon_pi_two_func_coeff,
        const std::vector<std::vector<std::vector<Sequential_Prop_1pt_Block_Collection>>> &block_coll,
        const std::vector<std::vector<std::vector<Sequential_Prop_1pt_Block_Collection>>> &block_coll_ex,
        const std::vector<std::vector<int>> &Momentum_Targets_src,
        const std::vector<std::vector<int>> &Momentum_Targets_curr,
        const bool &is_baryon_smear,
        const PointsSelection &psel_baryon,
        const long &psel_baryon_num,
        const std::vector<int> &psel_baryon_num_list,
        const std::vector<int> &list_n_from_idx_baryon,
        const std::vector<std::vector<long>> &idx_class_by_time_baryon,
        const bool &is_meson_smear,
        const PointsSelection &psel_meson,
        const long &psel_meson_num,
        const std::vector<int> &psel_meson_num_list,
        const std::vector<int> &list_n_from_idx_meson,
        const std::vector<std::vector<long>> &idx_class_by_time_meson,
        const std::vector<int> &fsel_num_list,
        const std::vector<int> &list_n_from_idx_fsel,
        const std::vector<std::vector<long>> &idx_class_by_time_fsel,
        const FieldSelection &fsel, const Geometry &geo,
        const int &ti, const int &dtmax, const int &dtmin,
        const int &t_baryon_snk, const int &xt, const int &t_meson, const int &t_baryon_src, const int &dtxy, const int &t_src_snk,
        std::vector<std::vector<ComplexD>> &leading_tiwst_result,
        const double &anti_period, const int &gram_num)
    {
        TIMER_VERBOSE("leading_tiwst_mate_fsel");

        std::vector<std::vector<std::vector<std::vector<std::vector<Sequential_Prop_2pt_Block>>>>> block_x_y;
        contract_sequential_block_x_fsel_y_psel_mate_ultra_magic(
            job_tag, traj,
            block_coll,
            block_coll_ex,
            block_x_y,
            is_baryon_smear,
            psel_baryon,
            psel_baryon_num,
            psel_baryon_num_list,
            list_n_from_idx_baryon,
            idx_class_by_time_baryon,
            is_meson_smear,
            psel_meson,
            psel_meson_num,
            psel_meson_num_list,
            list_n_from_idx_meson,
            idx_class_by_time_meson,
            fsel_num_list,
            list_n_from_idx_fsel,
            idx_class_by_time_fsel,
            fsel,
            geo,
            Momentum_Targets_curr,
            dtmax, dtmin,
            t_baryon_snk, xt, t_meson, t_baryon_src);

        std::vector<std::vector<Baryon_Block_No_Projection_Psel_Psel>> block;
        contract_baryon_pselpsel_no_projection_block(
            job_tag, traj,
            is_baryon_smear,
            psel_baryon,
            psel_baryon_num,
            psel_baryon_num_list,
            list_n_from_idx_baryon,
            idx_class_by_time_baryon,
            is_meson_smear,
            psel_meson,
            psel_meson_num,
            psel_meson_num_list,
            list_n_from_idx_meson,
            idx_class_by_time_meson,
            geo, ti, dtmax, dtmin,
            t_baryon_snk, xt, t_meson, t_baryon_src, dtxy,
            block);

        contract_leading_tiwst_topology_fsel_mate_ultra(
            job_tag, traj,
            block, block_x_y, baryon_pi_two_func_coeff,
            Momentum_Targets_src,
            Momentum_Targets_curr,
            is_baryon_smear,
            psel_baryon,
            psel_baryon_num,
            psel_baryon_num_list,
            list_n_from_idx_baryon,
            idx_class_by_time_baryon,
            is_meson_smear,
            psel_meson,
            psel_meson_num,
            psel_meson_num_list,
            list_n_from_idx_meson,
            idx_class_by_time_meson,
            fsel_num_list,
            list_n_from_idx_fsel,
            idx_class_by_time_fsel,
            fsel, geo, ti, dtmax, dtmin,
            t_baryon_snk, xt, t_meson, t_baryon_src, dtxy, t_src_snk,
            leading_tiwst_result, anti_period, gram_num);
        // displayln(ssprintf("thread_id: %d/%d, t_baryon_snk:%d, xt:%d, t_meson:%d, t_baryon_src:%d", omp_get_thread_num(), omp_get_num_threads(), t_baryon_snk, xt, t_meson, t_baryon_src));
    }

    inline void contract_highter_tiwst_topology(
        const std::string &job_tag, const int &traj,
        const std::vector<std::vector<std::vector<std::vector<Sequential_Prop_1pt_Block>>>> &block_x_fsel,
        std::vector<std::vector<std::vector<Sequential_Prop_1pt_Block>>> &block_y,
        const std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<ComplexD>>>>>> &baryon_pi_two_func_coeff,
        const std::vector<std::vector<int>> &Momentum_Targets_src,
        const std::vector<std::vector<int>> &Momentum_Targets_curr,
        const bool &is_baryon_smear,
        const PointsSelection &psel_baryon,
        const long &psel_baryon_num,
        const std::vector<int> &psel_baryon_num_list,
        const std::vector<int> &list_n_from_idx_baryon,
        const std::vector<std::vector<long>> &idx_class_by_time_baryon,
        const bool &is_meson_smear,
        const PointsSelection &psel_meson,
        const long &psel_meson_num,
        const std::vector<int> &psel_meson_num_list,
        const std::vector<int> &list_n_from_idx_meson,
        const std::vector<std::vector<long>> &idx_class_by_time_meson,
        const std::vector<int> &fsel_num_list,
        const std::vector<int> &list_n_from_idx_fsel,
        const std::vector<std::vector<long>> &idx_class_by_time_fsel,
        const FieldSelection &fsel, const Geometry &geo, const int &ti,
        const int &t_baryon_snk, const int &xt, const int &t_meson, const int &t_baryon_src, const int &dtxy, const int &t_src_snk,
        std::vector<std::vector<ComplexD>> &highter_tiwst_result,
        const double &anti_period,
        const int &gram_num)
    {
        TIMER_VERBOSE("contract_highter_tiwst_topology");
        const Coordinate &total_site = geo.total_site();

        const int &mom_num_src = Momentum_Targets_src.size();
        const int &mom_num_curr = Momentum_Targets_curr.size();

        std::vector<std::vector<std::vector<Baryon_SPBlock_No_Projection_Psel_Psel>>> spblock;
        spblock.resize(psel_baryon_num_list[t_baryon_src]);
        for (int idx_baryon_src = 0; idx_baryon_src < psel_baryon_num_list[t_baryon_src]; idx_baryon_src++)
        {
            spblock[idx_baryon_src].resize(psel_baryon_num_list[t_baryon_snk]);
            for (int idx_baryon_snk = 0; idx_baryon_snk < psel_baryon_num_list[t_baryon_snk]; idx_baryon_snk++)
            {
                spblock[idx_baryon_src][idx_baryon_snk].resize(psel_meson_num_list[t_meson]);
            }
        }

#pragma omp parallel for collapse(2)
        for (int idx_baryon_src = 0; idx_baryon_src < psel_baryon_num_list[t_baryon_src]; idx_baryon_src++)
        {
            for (int idx_baryon_snk = 0; idx_baryon_snk < psel_baryon_num_list[t_baryon_snk]; idx_baryon_snk++)
            {
                const long &xg_src_psel_idx = idx_class_by_time_baryon[t_baryon_src][idx_baryon_src];
                const Coordinate &xg_src = psel_baryon[xg_src_psel_idx];
                const PselProp &prop_src = get_psel_prop(job_tag, traj, xg_src, is_baryon_smear, is_baryon_smear);
                // qassert(t_baryon_src == xg_src[3]);

                const long &xg_snk_psel_idx = idx_class_by_time_baryon[t_baryon_snk][idx_baryon_snk];
                const Coordinate &xg_snk = psel_baryon[xg_snk_psel_idx];

                for (int idx_meson_ty = 0; idx_meson_ty < psel_meson_num_list[t_meson]; idx_meson_ty++)
                {
                    const long &xg_y_psel_idx = idx_class_by_time_meson[t_meson][idx_meson_ty];
                    const Coordinate &xg_y = psel_meson[xg_y_psel_idx];

                    // qassert(list_n_from_idx_baryon[xg_src_psel_idx] == idx_baryon_src);
                    // qassert(list_n_from_idx_baryon[xg_snk_psel_idx] == idx_baryon_snk);
                    // qassert(list_n_from_idx_meson[xg_y_psel_idx] == idx_meson_ty);

                    Baryon_SPBlock_No_Projection_Psel_Psel &spblock_np = spblock[idx_baryon_src][idx_baryon_snk][idx_meson_ty];

                    Sequential_Prop_1pt_Block &block_y_temp = block_y[idx_baryon_snk][idx_baryon_src][idx_meson_ty];
                    // qassert(block_y_temp.xg_par2_psel_idx == xg_y_psel_idx);

                    const WilsonMatrix &wm = prop_src.get_elem(xg_snk_psel_idx);
                    const WilsonMatrix &wm_ysq = block_y_temp.block;
                    const SpinMatrix &Cg5 = BaryonMatrixConstants::get_Cgamma5();

                    const WilsonMatrix &Cg5_wm = Cg5 * wm;
                    const WilsonMatrix &Cg5_wm_ysq = Cg5 * wm_ysq;

                    const WilsonMatrix &wm_Cg5 = wm * Cg5;
                    const WilsonMatrix &wm_ysq_Cg5 = wm_ysq * Cg5;

                    const WilsonMatrix &Cg5_wm_Cg5 = Cg5_wm * Cg5;
                    const WilsonMatrix &Cg5_wm_ysq_Cg5 = Cg5_wm_ysq * Cg5;

                    baryon_pi_no_projection_block_0_3(spblock_np.spblock_5, Cg5_wm_Cg5, wm_ysq);
                    baryon_pi_no_projection_block_0_12(spblock_np.spblock_6, Cg5_wm_ysq_Cg5, wm);
                    baryon_pi_no_projection_block_1_2(spblock_np.spblock_7, Cg5_wm_ysq_Cg5, wm);
                    baryon_pi_no_projection_block_1_3(spblock_np.spblock_8, Cg5_wm_ysq_Cg5, wm);
                    baryon_pi_no_projection_block_1_2(spblock_np.spblock_9, Cg5_wm_Cg5, wm_ysq);

                    baryon_pi_no_projection_block_0_12(spblock_np.spblock_ex_5, Cg5_wm_Cg5, wm_ysq);
                    baryon_pi_no_projection_block_0_12(spblock_np.spblock_ex_6, Cg5_wm_ysq_Cg5, wm);
                    baryon_pi_no_projection_block_1_1(spblock_np.spblock_ex_7, wm_ysq_Cg5, Cg5_wm);
                    baryon_pi_no_projection_block_1_1(spblock_np.spblock_ex_8, wm_Cg5, Cg5_wm_ysq);
                    baryon_pi_no_projection_block_1_3(spblock_np.spblock_ex_9, Cg5_wm_Cg5, wm_ysq);

                    spblock_np.xg_snk_psel_idx = xg_snk_psel_idx;
                    spblock_np.xg_y_psel_idx = xg_y_psel_idx;
                    spblock_np.xg_src_psel_idx = xg_src_psel_idx;
                    spblock_np.n_snk = list_n_from_idx_baryon[xg_snk_psel_idx];
                    spblock_np.n_src = list_n_from_idx_baryon[xg_src_psel_idx];
                    spblock_np.is_build = true;
                }
            }
        }

#pragma omp parallel for collapse(2)
        for (int idx_baryon_src = 0; idx_baryon_src < psel_baryon_num_list[t_baryon_src]; idx_baryon_src++)
        {
            for (int idx_baryon_snk = 0; idx_baryon_snk < psel_baryon_num_list[t_baryon_snk]; idx_baryon_snk++)
            {
                const long &xg_src_psel_idx = idx_class_by_time_baryon[t_baryon_src][idx_baryon_src];
                const Coordinate &xg_src = psel_baryon[xg_src_psel_idx];
                const long &xg_snk_psel_idx = idx_class_by_time_baryon[t_baryon_snk][idx_baryon_snk];
                const Coordinate &xg_snk = psel_baryon[xg_snk_psel_idx];
                for (int idx_meson_ty = 0; idx_meson_ty < psel_meson_num_list[t_meson]; idx_meson_ty++)
                {
                    const long &xg_y_psel_idx = idx_class_by_time_meson[t_meson][idx_meson_ty];
                    const Coordinate &xg_y = psel_meson[xg_y_psel_idx];

                    if (is_baryon_smear == is_meson_smear && (xg_snk_psel_idx == xg_y_psel_idx || xg_src_psel_idx == xg_y_psel_idx))
                    {
                        continue;
                    }
                    // qassert(list_n_from_idx_baryon[xg_src_psel_idx] == idx_baryon_src);
                    // qassert(list_n_from_idx_baryon[xg_snk_psel_idx] == idx_baryon_snk);
                    // qassert(list_n_from_idx_meson[xg_y_psel_idx] == idx_meson_ty);

                    const Baryon_SPBlock_No_Projection_Psel_Psel &spblock_np = spblock[idx_baryon_src][idx_baryon_snk][idx_meson_ty];
                    // qassert(spblock_np.is_build);
                    // qassert(spblock_np.xg_src_psel_idx == xg_src_psel_idx);
                    // qassert(spblock_np.xg_y_psel_idx == xg_y_psel_idx);
                    // qassert(spblock_np.xg_snk_psel_idx == xg_snk_psel_idx);
                    // if (0 == fsel_num_list[xt])
                    // {
                    //     continue;
                    // }

                    for (int mom_curr_id = 0; mom_curr_id < mom_num_curr; mom_curr_id++)
                    {
                        // long xg_x_fsel_idx = idx_class_by_time_fsel[xt][idx_x];
                        // const long index = fsel.indices[xg_x_fsel_idx];
                        // const Coordinate xg_x_l = geo.coordinate_from_index(index);
                        // const Coordinate xg_x_g = geo.coordinate_g_from_l(xg_x_l);
                        // const Coordinate &xg_x = xg_x_g;
                        // const int &t_x = xg_x[3];
                        // qassert(t_x == xt);
                        // qassert(idx_x == list_n_from_idx_fsel[xg_x_fsel_idx]);

                        for (int VA_idx = 0; VA_idx < 8; VA_idx++)
                        {
                            const Sequential_Prop_1pt_Block &block_x_temp = block_x_fsel[idx_baryon_snk][idx_baryon_src][mom_curr_id][VA_idx];
                            // qassert(block_x_temp.xg_par1_psel_idx == xg_snk_psel_idx);
                            // qassert(block_x_temp.xg_par2_psel_idx == xg_x_psel_idx);
                            // qassert(block_x_temp.xg_par3_psel_idx == xg_src_psel_idx);
                            const WilsonMatrix &wm_xsq = block_x_temp.block;

                            std::vector<SpinMatrix> spin_result_list_temp;
                            contract_swm_spblock_np(spin_result_list_temp, spblock_np, wm_xsq);

                            for (int mom_src_id = 0; mom_src_id < mom_num_src; ++mom_src_id)
                            {
                                long phase_temp = space_dot(Momentum_Targets_src[mom_src_id], xg_src) - space_dot(Momentum_Targets_src[mom_src_id], xg_y) + space_dot(Momentum_Targets_curr[mom_curr_id], xg_snk);

                                int phase = mod(phase_temp, total_site[0]);
                                const ComplexD normal_coeff = baryon_pi_two_func_coeff[ti][t_src_snk][mom_src_id][mom_curr_id][dtxy][phase] * (ComplexD)anti_period;

                                for (int topology_id = 0; topology_id < gram_num; topology_id++)
                                {
                                    SpinMatrix spin_result_list = spin_result_list_temp[topology_id] * normal_coeff;
                                    for (int mu1 = 0; mu1 < 4; mu1++)
                                    {
                                        for (int mu2 = 0; mu2 < 4; mu2++)
                                        {
                                            if (spin_result_list(mu1, mu2) == 0.0)
                                            {
                                                continue;
                                            }
                                            highter_tiwst_result[omp_get_thread_num()][((((mom_src_id * mom_num_curr + mom_curr_id) * gram_num + topology_id) * 8 + VA_idx) * 4 + mu1) * 4 + mu2] += spin_result_list(mu1, mu2);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    inline void highter_tiwst_fsel_mate(
        const std::string &job_tag, const int &traj,
        const std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<ComplexD>>>>>> &baryon_pi_two_func_coeff,
        const std::vector<std::vector<std::vector<Sequential_Prop_1pt_Block_Collection>>> &block_coll,
        const std::vector<std::vector<int>> &Momentum_Targets_src,
        const std::vector<std::vector<int>> &Momentum_Targets_curr,
        const bool &is_baryon_smear,
        const PointsSelection &psel_baryon,
        const long &psel_baryon_num,
        const std::vector<int> &psel_baryon_num_list,
        const std::vector<int> &list_n_from_idx_baryon,
        const std::vector<std::vector<long>> &idx_class_by_time_baryon,
        const bool &is_meson_smear,
        const PointsSelection &psel_meson,
        const long &psel_meson_num,
        const std::vector<int> &psel_meson_num_list,
        const std::vector<int> &list_n_from_idx_meson,
        const std::vector<std::vector<long>> &idx_class_by_time_meson,
        const std::vector<int> &fsel_num_list,
        const std::vector<int> &list_n_from_idx_fsel,
        const std::vector<std::vector<long>> &idx_class_by_time_fsel,
        const FieldSelection &fsel, const Geometry &geo,
        const int &ti, const int &dtmax, const int &dtmin,
        const int &t_baryon_snk, const int &xt, const int &t_meson, const int &t_baryon_src, const int &dtxy, const int &t_src_snk,
        std::vector<std::vector<ComplexD>> &highter_tiwst_result,
        const double &anti_period, const int &gram_num)
    {
        TIMER_VERBOSE("highter_tiwst_fsel_mate");
        const Coordinate &total_site = geo.total_site();
        std::vector<std::vector<std::vector<Sequential_Prop_1pt_Block>>> block_y;

        contract_sequential_block_psel(
            job_tag, traj, block_y,
            Momentum_Targets_src,
            is_baryon_smear,
            psel_baryon,
            psel_baryon_num,
            psel_baryon_num_list,
            list_n_from_idx_baryon,
            idx_class_by_time_baryon,
            is_meson_smear,
            psel_meson,
            psel_meson_num,
            psel_meson_num_list,
            list_n_from_idx_meson,
            idx_class_by_time_meson,
            geo,
            t_baryon_snk, t_meson, t_baryon_src,
            true, false, true);

        const Sequential_Prop_1pt_Block_Collection &block_x_fsel_ = block_coll[t_baryon_snk][mod(t_baryon_snk - t_baryon_src, total_site[3]) - dtmin][mod(xt - t_baryon_src, total_site[3])];
        qassert(block_x_fsel_.is_build);
        qassert(block_x_fsel_.tslice_1 == t_baryon_snk);
        qassert(block_x_fsel_.tslice_2 == xt);
        qassert(block_x_fsel_.tslice_3 == t_baryon_src);

        const std::vector<std::vector<std::vector<std::vector<Sequential_Prop_1pt_Block>>>> block_x_fsel = block_x_fsel_.block_coll;

        contract_highter_tiwst_topology(
            job_tag, traj,
            block_x_fsel,
            block_y,
            baryon_pi_two_func_coeff,
            Momentum_Targets_src,
            Momentum_Targets_curr,
            is_baryon_smear,
            psel_baryon,
            psel_baryon_num,
            psel_baryon_num_list,
            list_n_from_idx_baryon,
            idx_class_by_time_baryon,
            is_meson_smear,
            psel_meson,
            psel_meson_num,
            psel_meson_num_list,
            list_n_from_idx_meson,
            idx_class_by_time_meson,
            fsel_num_list,
            list_n_from_idx_fsel,
            idx_class_by_time_fsel,
            fsel, geo,
            ti,
            t_baryon_snk, xt, t_meson, t_baryon_src, dtxy, t_src_snk,
            highter_tiwst_result,
            anti_period, gram_num);
    }

    inline void acc_data_to_LatData_fsel(
        const std::vector<std::vector<ComplexD>> &leading_tiwst_result,
        const std::vector<std::vector<ComplexD>> &highter_tiwst_result,
        LatData &baryon_pi_data_temp,
        const std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<ComplexD>>>>>> &baryon_pi_two_func_coeff,
        const std::vector<std::vector<int>> &Momentum_Targets_src,
        const std::vector<std::vector<int>> &Momentum_Targets_curr,
        const bool &is_baryon_smear,
        const PointsSelection &psel_baryon,
        const long &psel_baryon_num,
        const std::vector<int> &psel_baryon_num_list,
        const std::vector<int> &list_n_from_idx_baryon,
        const std::vector<std::vector<long>> &idx_class_by_time_baryon,
        const bool &is_meson_smear,
        const PointsSelection &psel_meson,
        const long &psel_meson_num,
        const std::vector<int> &psel_meson_num_list,
        const std::vector<int> &list_n_from_idx_meson,
        const std::vector<std::vector<long>> &idx_class_by_time_meson,
        const std::vector<int> &fsel_num_list,
        const std::vector<int> &list_n_from_idx_fsel,
        const std::vector<std::vector<long>> &idx_class_by_time_fsel,
        const Geometry &geo, const int &ti,
        const int &dtmax, const int &dtmin,
        const int &t_baryon_snk, const int &xt, const int &t_meson, const int &t_baryon_src, const int &dtxy, const int &t_src_snk, const int &gram_num)
    {
        TIMER_VERBOSE("acc_data_to_LatData_fsel");

        const Coordinate &total_site = geo.total_site();
        const int &mom_num_src = Momentum_Targets_src.size();
        const int &mom_num_curr = Momentum_Targets_curr.size();

#pragma omp parallel for collapse(2)
        for (int mom_src_id = 0; mom_src_id < mom_num_src; ++mom_src_id)
        {
            for (int mom_curr_id = 0; mom_curr_id < mom_num_curr; ++mom_curr_id)
            {
                Vector<ComplexD> baryon_pi_data_vec_temp = lat_data_complex_get(baryon_pi_data_temp, make_array(dtxy, mom_src_id, mom_curr_id));
                for (int thread_id = 0; thread_id < omp_get_max_threads(); thread_id++)
                {
                    for (int topology_id = 0; topology_id < gram_num; topology_id++)
                    {
                        for (int VA_idx = 0; VA_idx < 8; VA_idx++)
                        {
                            for (int mu1 = 0; mu1 < 4; mu1++)
                            {
                                for (int mu2 = 0; mu2 < 4; mu2++)
                                {
                                    baryon_pi_data_vec_temp[((topology_id * 8 + VA_idx) * 4 + mu1) * 4 + mu2] += leading_tiwst_result[thread_id][((((mom_src_id * mom_num_curr + mom_curr_id) * gram_num + topology_id) * 8 + VA_idx) * 4 + mu1) * 4 + mu2];
                                    baryon_pi_data_vec_temp[((((topology_id + gram_num) * 8 + VA_idx)) * 4 + mu1) * 4 + mu2] += highter_tiwst_result[thread_id][((((mom_src_id * mom_num_curr + mom_curr_id) * gram_num + topology_id) * 8 + VA_idx) * 4 + mu1) * 4 + mu2];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    inline void compute_baryon_pi_rescattering_fsel_arbitrarty_func_psel_type(
        const std::string &job_tag, const int &traj,
        const std::vector<std::vector<int>> &Momentum_Targets_src,
        const std::vector<std::vector<int>> &Momentum_Targets_curr,
        const std::vector<int> &tsep_src_list, const std::vector<int> &tsep_snk_list,
        const bool is_baryon_smear, const bool is_meson_smear)
    {
        check_sigterm();
        check_time_limit();
        Timer::autodisplay();
        const int gram_num = 10;

        qassert(tsep_src_list.size() == tsep_snk_list.size());
        const int tsep_num = (int)tsep_src_list.size();

        TIMER_VERBOSE("compute_baryon_pi_rescattering_fsel_arbitrarty_func_psel_type");

        const PointsSelection &psel = get_point_selection(job_tag, traj);
        const long n_points = psel.size();
        const PointsSelection &psel_smear = get_point_selection_smear(job_tag, traj);
        const long n_points_smear = psel_smear.size();

        const PointsSelection &psel_meson = (!is_meson_smear ? psel : psel_smear);
        const long &psel_meson_num = (!is_meson_smear ? n_points : n_points_smear);
        const PointsSelection &psel_baryon = (!is_baryon_smear ? psel : psel_smear);
        const long &psel_baryon_num = (!is_baryon_smear ? n_points : n_points_smear);

        const FieldSelection &fsel = get_field_selection(job_tag, traj);

        const Geometry &geo = fsel.f_rank.geo();
        const Coordinate total_site = geo.total_site();
        const int num_dtxy = get_baryon_pi_half_sep(total_site[3]);
        const int &dtmax = num_dtxy + (tsep_snk_list.back() > 0 ? tsep_snk_list.back() : 0) + (tsep_src_list.back() > 0 ? tsep_src_list.back() : 0);
        const int &dtmin = +(tsep_snk_list.front() > 0 ? tsep_snk_list.front() : 0) + (tsep_src_list.front() > 0 ? tsep_src_list.front() : 0); // block 所需时间片长度

        const std::string path = get_baryon_pi_rescattering_all_psel_arbitrarty_func_psel_path(job_tag, traj);
        std::vector<std::vector<std::string>> fn_gram_list(tsep_num);
        for (int ti = 0; ti < tsep_num; ++ti)
        {
            const int &tsep_src = tsep_src_list[ti];
            const int &tsep_snk = tsep_snk_list[ti];

            fn_gram_list[ti].resize(num_dtxy);
            for (int t_src_snk = 0; t_src_snk < num_dtxy; ++t_src_snk)
            {
                fn_gram_list[ti][t_src_snk] = path + ssprintf("/baryon_%s_pi_%s_rescattering_mate_ultra_magic_fsel_tsep_%d_%d_tss_%d.field", (is_baryon_smear ? "smear" : "point"), (is_meson_smear ? "smear" : "point"), tsep_src, tsep_snk, t_src_snk + tsep_src + tsep_snk);
            }
        }

        bool is_complete = true;
        for (int ti = 0; ti < tsep_num; ++ti)
        {
            for (int t_src_snk = 0; t_src_snk < num_dtxy; ++t_src_snk)
            {
                if (fn_gram_list[ti][t_src_snk] != "")
                {
                    if (not is_d_field(fn_gram_list[ti][t_src_snk]))
                    {
                        is_complete = false;
                        break;
                    }
                }
            }
        }

        if (is_complete)
        {
            return;
        }

        std::vector<std::vector<LatData>> baryon_pi_data_result(tsep_num);
        for (int ti = 0; ti < tsep_num; ++ti)
        {
            baryon_pi_data_result[ti].resize(num_dtxy);
            for (int t_src_snk = 0; t_src_snk < num_dtxy; ++t_src_snk)
            {
                baryon_pi_data_result[ti][t_src_snk] = mk_baryon_pi_rescattering_fsel_for_arbitrary_mate(t_src_snk, gram_num, Momentum_Targets_src, Momentum_Targets_curr, total_site);
            }
        }

        std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<ComplexD>>>>>> baryon_pi_two_func_coeff;
        std::vector<int> psel_baryon_num_list;
        std::vector<int> psel_meson_num_list;
        std::vector<int> fsel_num_list;
        std::vector<int> list_n_from_idx_baryon(psel_baryon_num);
        std::vector<int> list_n_from_idx_meson(psel_meson_num);
        std::vector<int> list_n_from_idx_fsel(fsel.n_elems);
        std::vector<std::vector<long>> idx_class_by_time_baryon(total_site[3]);
        std::vector<std::vector<long>> idx_class_by_time_meson(total_site[3]);
        std::vector<std::vector<long>> idx_class_by_time_fsel(total_site[3]);

        baryon_pion_two_point_arbitrary_coeffience_mate_magic_fsel(
            job_tag, traj,
            baryon_pi_two_func_coeff,
            psel_baryon_num_list,
            psel_meson_num_list,
            list_n_from_idx_baryon,
            list_n_from_idx_meson,
            idx_class_by_time_baryon,
            idx_class_by_time_meson,
            fsel_num_list,
            list_n_from_idx_fsel,
            idx_class_by_time_fsel,
            Momentum_Targets_src,
            Momentum_Targets_curr,
            fsel, geo,
            psel_baryon, psel_meson,
            tsep_src_list, tsep_snk_list,
            is_baryon_smear, is_meson_smear);

        for (long nsrc = 0; nsrc < n_points; ++nsrc)
        {
            const long &xg_src_psel_idx = nsrc;
            const Coordinate xg_src = psel_smear[xg_src_psel_idx];
            const PselProp &prop_src = get_psel_prop_smear(job_tag, traj, xg_src, 0, 0, 1);
        }

        displayln_info("baryon_pion_two_point_coeffience_finish;");

        qassert(is_baryon_smear == is_meson_smear);
        std::vector<std::vector<std::vector<Sequential_Prop_1pt_Block_Collection>>> block_coll;
        contract_sequential_block_mate_coll(
            job_tag, traj,
            block_coll,
            is_baryon_smear,
            psel_baryon,
            psel_baryon_num,
            psel_baryon_num_list,
            list_n_from_idx_baryon,
            idx_class_by_time_baryon,
            is_meson_smear,
            psel_meson,
            psel_meson_num,
            psel_meson_num_list,
            list_n_from_idx_meson,
            idx_class_by_time_meson,
            fsel_num_list,
            list_n_from_idx_fsel,
            idx_class_by_time_fsel,
            fsel,
            geo,
            true, true,
            Momentum_Targets_curr,
            dtmin, dtmax, tsep_src_list);

        std::vector<std::vector<std::vector<Sequential_Prop_1pt_Block_Collection>>> block_coll_ex;
        contract_sequential_block_mate_ultra_coll(
            job_tag, traj,
            block_coll_ex,
            is_baryon_smear,
            psel_baryon,
            psel_baryon_num,
            psel_baryon_num_list,
            list_n_from_idx_baryon,
            idx_class_by_time_baryon,
            is_meson_smear,
            psel_meson,
            psel_meson_num,
            psel_meson_num_list,
            list_n_from_idx_meson,
            idx_class_by_time_meson,
            fsel_num_list,
            list_n_from_idx_fsel,
            idx_class_by_time_fsel,
            fsel,
            geo,
            true, true,
            Momentum_Targets_curr,
            dtmin, dtmax, tsep_src_list);
        displayln_info("contract_sequential_block_mate_coll;");
        glb_sum(block_coll);
        displayln_info("glb_sum_finish 0");
        glb_sum(block_coll_ex);
        displayln_info("glb_sum_finish 1");
        MPI_Barrier(MPI_COMM_WORLD);
        displayln_info("glb_sum_finish");
        Timer::autodisplay();

        const int y_idx_sparsity = 1;
        for (int ti = 0; ti < tsep_num; ++ti)
        {
            const int &tsep_src = tsep_src_list[ti];
            const int &tsep_snk = tsep_snk_list[ti];
            for (int t_src_snk = 0; t_src_snk < num_dtxy; ++t_src_snk)
            {
                long iter = 0;
                for (int t_baryon_src = 0; t_baryon_src < total_site[3]; ++t_baryon_src)
                {
                    // 获取通信域中的进程总数（节点数）
                    int MPI_SIZE;
                    MPI_Comm_size(MPI_COMM_WORLD, &MPI_SIZE);

                    // 获取当前进程的进程号
                    int MPI_RANK;
                    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_RANK);

                    if (t_baryon_src % MPI_SIZE != MPI_RANK)
                    {
                        continue;
                    }
                    for (int dt = 0; dt < t_src_snk; ++dt)
                    {
                        const int &dtxy = dt;
                        int xt, t_meson, t_baryon_snk;
                        double anti_period;
                        three_point_time_slice(dtxy, t_src_snk, tsep_src, tsep_snk, total_site, t_meson, t_baryon_src, t_baryon_snk, xt, anti_period);

                        Timer::autodisplay();
                        TIMER_VERBOSE("compute_baryon_pi_rescattering_all_psel_func_noama_type-iter");

                        std::vector<std::vector<ComplexD>> leading_tiwst_result;
                        std::vector<std::vector<ComplexD>> highter_tiwst_result;

                        const int &mom_num_src = Momentum_Targets_src.size();
                        const int &mom_num_curr = Momentum_Targets_curr.size();

                        leading_tiwst_result.resize(omp_get_max_threads());
                        highter_tiwst_result.resize(omp_get_max_threads());
                        for (int thread_id = 0; thread_id < omp_get_max_threads(); thread_id++)
                        {
                            leading_tiwst_result[thread_id].resize(mom_num_src * mom_num_curr * gram_num * 8 * 4 * 4);
                            highter_tiwst_result[thread_id].resize(mom_num_src * mom_num_curr * gram_num * 8 * 4 * 4);
                            set_zero(leading_tiwst_result[thread_id]);
                            set_zero(highter_tiwst_result[thread_id]);
                        }

                        leading_tiwst_mate_fsel(
                            job_tag, traj,
                            baryon_pi_two_func_coeff,
                            block_coll,
                            block_coll_ex,
                            Momentum_Targets_src,
                            Momentum_Targets_curr,
                            is_baryon_smear,
                            psel_baryon,
                            psel_baryon_num,
                            psel_baryon_num_list,
                            list_n_from_idx_baryon,
                            idx_class_by_time_baryon,
                            is_meson_smear,
                            psel_meson,
                            psel_meson_num,
                            psel_meson_num_list,
                            list_n_from_idx_meson,
                            idx_class_by_time_meson,
                            fsel_num_list,
                            list_n_from_idx_fsel,
                            idx_class_by_time_fsel,
                            fsel, geo, ti, dtmax, dtmin,
                            t_baryon_snk, xt, t_meson, t_baryon_src, dtxy, t_src_snk,
                            leading_tiwst_result, anti_period, gram_num);

                        highter_tiwst_fsel_mate(
                            job_tag, traj,
                            baryon_pi_two_func_coeff,
                            block_coll,
                            Momentum_Targets_src,
                            Momentum_Targets_curr,
                            is_baryon_smear,
                            psel_baryon,
                            psel_baryon_num,
                            psel_baryon_num_list,
                            list_n_from_idx_baryon,
                            idx_class_by_time_baryon,
                            is_meson_smear,
                            psel_meson,
                            psel_meson_num,
                            psel_meson_num_list,
                            list_n_from_idx_meson,
                            idx_class_by_time_meson,
                            fsel_num_list,
                            list_n_from_idx_fsel,
                            idx_class_by_time_fsel,
                            fsel, geo, ti, dtmax, dtmin,
                            t_baryon_snk, xt, t_meson, t_baryon_src, dtxy, t_src_snk,
                            highter_tiwst_result, anti_period, gram_num);

                        acc_data_to_LatData_fsel(
                            leading_tiwst_result,
                            highter_tiwst_result,
                            baryon_pi_data_result[ti][t_src_snk],
                            baryon_pi_two_func_coeff,
                            Momentum_Targets_src,
                            Momentum_Targets_curr,
                            is_baryon_smear,
                            psel_baryon,
                            psel_baryon_num,
                            psel_baryon_num_list,
                            list_n_from_idx_baryon,
                            idx_class_by_time_baryon,
                            is_meson_smear,
                            psel_meson,
                            psel_meson_num,
                            psel_meson_num_list,
                            list_n_from_idx_meson,
                            idx_class_by_time_meson,
                            fsel_num_list,
                            list_n_from_idx_fsel,
                            idx_class_by_time_fsel,
                            geo, ti, dtmax, dtmin,
                            t_baryon_snk, xt, t_meson, t_baryon_src, dtxy, t_src_snk, gram_num);

                        iter += 1;
                        displayln_info(fname + ssprintf(":iter=%ld ti=%ld t_src_snk=%ld tmeson=%ld dt=%ld", iter, ti, t_src_snk, t_meson, dt));
                    }
                }
                // MPI_Barrier(MPI_COMM_WORLD);
                glb_sum(baryon_pi_data_result[ti][t_src_snk]);
                lat_data_save_info(fn_gram_list[ti][t_src_snk], baryon_pi_data_result[ti][t_src_snk]);
                Timer::autodisplay();
            }
        }
    }

    inline void compute_baryon_pi_rescattering_fsel_arbitrarty_func_psel(const std::string &job_tag,
                                                                                    const int &traj, const bool is_baryon_smear, const bool is_meson_smear)
    {
        check_sigterm();
        Timer::autodisplay();
        TIMER_VERBOSE("compute_baryon_pi_rescattering_fsel_arbitrarty_func_psel");
        const std::string path = get_baryon_pi_rescattering_all_psel_arbitrarty_func_psel_path(job_tag, traj);
        const std::string path_checkpoint = path + "/checkpoint.txt";
        if (does_file_exist_sync_node(path_checkpoint))
        {
            return;
        }
        if (does_file_exist_sync_node(path + ssprintf("/baryon_%s_pi_%s_rescattering_mate_ultra_magic_fsel_tsep_%d_%d_tss_%d.field", (is_baryon_smear ? "smear" : "point"), (is_meson_smear ? "smear" : "point"), 0, 0, 1)))
        {
            return;
        }
        if (not(check_prop_smear(job_tag, traj, 0)))
        {
            displayln_info("err_check_prop_smear;");
            displayln_info(get_prop_smear_path(job_tag, traj, 0));
            displayln_info(get_psel_prop_smear_path(job_tag, traj, 0));
            return;
        }
        check_sigterm();
        check_time_limit();
        if (not obtain_lock(
                ssprintf("lock-baryon-pi-%s-%d", job_tag.c_str(), traj)))
        {
            return;
        }
        setup(job_tag, traj);
        qmkdir_info("analysis/baryon_pi_rescattering_fsel_mate_ultra_magic_pro");
        qmkdir_info(ssprintf("analysis/baryon_pi_rescattering_fsel_mate_ultra_magic_pro/%s", job_tag.c_str()));
        qmkdir_info(path);

        std::vector<std::vector<int>> Momentum_Targets_src;

        Momentum_Targets_src.push_back({0, 0, 0});

        Momentum_Targets_src.push_back({0, 0, 1});
        Momentum_Targets_src.push_back({0, 0, -1});
        Momentum_Targets_src.push_back({0, 1, 0});
        Momentum_Targets_src.push_back({0, -1, 0});
        Momentum_Targets_src.push_back({1, 0, 0});
        Momentum_Targets_src.push_back({-1, 0, 0});

        Momentum_Targets_src.push_back({0, 1, 1});
        Momentum_Targets_src.push_back({0, 1, -1});
        Momentum_Targets_src.push_back({0, -1, 1});
        Momentum_Targets_src.push_back({0, -1, -1});
        Momentum_Targets_src.push_back({1, 0, 1});
        Momentum_Targets_src.push_back({1, 0, -1});
        Momentum_Targets_src.push_back({1, 1, 0});
        Momentum_Targets_src.push_back({1, -1, 0});
        Momentum_Targets_src.push_back({-1, 0, 1});
        Momentum_Targets_src.push_back({-1, 0, -1});
        Momentum_Targets_src.push_back({-1, 1, 0});
        Momentum_Targets_src.push_back({-1, -1, 0});

        std::vector<std::vector<int>> Momentum_Targets_curr;
        Momentum_Targets_curr.push_back({0, 0, 0});

        Momentum_Targets_curr.push_back({0, 0, 1});
        // Momentum_Targets_curr.push_back({0, 0, -1});
        // Momentum_Targets_curr.push_back({0, 1, 0});
        // Momentum_Targets_curr.push_back({0, -1, 0});
        // Momentum_Targets_curr.push_back({1, 0, 0});
        // Momentum_Targets_curr.push_back({-1, 0, 0});

        // Momentum_Targets_curr.push_back({0, 1, 1});
        // Momentum_Targets_curr.push_back({0, 1, -1});
        // Momentum_Targets_curr.push_back({0, -1, 1});
        // Momentum_Targets_curr.push_back({0, -1, -1});
        // Momentum_Targets_curr.push_back({1, 0, 1});
        // Momentum_Targets_curr.push_back({1, 0, -1});
        // Momentum_Targets_curr.push_back({1, 1, 0});
        // Momentum_Targets_curr.push_back({1, -1, 0});
        // Momentum_Targets_curr.push_back({-1, 0, 1});
        // Momentum_Targets_curr.push_back({-1, 0, -1});
        // Momentum_Targets_curr.push_back({-1, 1, 0});
        // Momentum_Targets_curr.push_back({-1, -1, 0});

        std::vector<int> tsep_src_list;
        tsep_src_list.push_back(0);

        std::vector<int> tsep_snk_list;
        tsep_snk_list.push_back(0);

        compute_baryon_pi_rescattering_fsel_arbitrarty_func_psel_type(
            job_tag, traj,
            Momentum_Targets_src,
            Momentum_Targets_curr,
            tsep_src_list, tsep_snk_list,
            is_baryon_smear, is_meson_smear);

        release_lock();
    }
}
