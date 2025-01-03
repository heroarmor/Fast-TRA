import matplotlib.pyplot as plt
import numpy as np

def parse_line(line):
    """
    解析形如:
    (0, 0) -> (1, 0) Total c: 259.43
    的字符串, 返回: ((0, 0), (1, 0), 259.43)
    """
    # 先按 " Total c: " 分割
    path_part, cost_part = line.strip().split(" Total c: ")
    cost = float(cost_part)

    # path_part 形如 "(0, 0) -> (1, 0)"
    node_from_str, node_to_str = path_part.split(" -> ")
    
    # 再去掉括号，得到 "0, 0"
    node_from_str = node_from_str.strip("() ")
    node_to_str   = node_to_str.strip("() ")

    # 拆分成整数坐标
    from_x, from_y = [int(x) for x in node_from_str.split(",")]
    to_x,   to_y   = [int(x) for x in node_to_str.split(",")]

    return (from_x, from_y), (to_x, to_y), cost


def draw_grid_with_edges(filename="result.txt"):
    # 读取文件
    edges = []          # 用于存储三元组: (node_from, node_to, cost)
    nodes_set = set()   # 用于收集出现过的所有节点
    
    with open(filename, "r", encoding="utf-8") as f:
        lines = f.readlines()
        for line in lines:
            # 如果行不空且包含 "Total c"
            line = line.strip()
            if line and "Total c:" in line:
                n_from, n_to, cost = parse_line(line)
                edges.append((n_from, n_to, cost))
                nodes_set.add(n_from)
                nodes_set.add(n_to)
    
    # 将所有节点的坐标提取出来，构造一个 dict: {node: (x, y)}
    # 这里我们直接将 (i,j) 节点画在 (i,j) 的位置
    pos = {node: (node[0], node[1]) for node in nodes_set}
    
    # 提取所有 cost 以确定颜色映射范围
    costs = [e[2] for e in edges]
    min_cost = min(costs)
    max_cost = max(costs)
    
    # 建立 colormap
    cmap = plt.cm.viridis  # 你也可以换成其他喜欢的 cmap
    norm = plt.Normalize(vmin=min_cost, vmax=max_cost)
    
    # 开始画图
    fig, ax = plt.subplots(figsize=(6, 6))
    
    # 先画节点（用方形/圆形都行，这里示例用圆）
    xs = [pos[node][0] for node in nodes_set]
    ys = [pos[node][1] for node in nodes_set]
    ax.scatter(xs, ys, color="black", s=50, zorder=3)  # zorder大一些，以免被边线覆盖
    
    # 给节点加上标注(可选)
    for node in nodes_set:
        ax.text(pos[node][0], pos[node][1], f"{node}",
                fontsize=8, ha='center', va='bottom', color='red')
    
    # 再画边
    for (n_from, n_to, cost) in edges:
        x0, y0 = pos[n_from]
        x1, y1 = pos[n_to]
        # 根据 cost 计算颜色
        edge_color = cmap(norm(cost))
        # 画线
        ax.plot([x0, x1], [y0, y1], color=edge_color, lw=3, zorder=2)
    
    # 加上颜色条
    sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)
    sm.set_array([])  # 不用真实数据也能生成 colorbar
    cbar = plt.colorbar(sm, ax=ax, pad=0.02)
    cbar.set_label("Total c")
    
    # 根据节点的范围来设定坐标轴范围(留点空白边距)
    all_x = [pos[n][0] for n in nodes_set]
    all_y = [pos[n][1] for n in nodes_set]
    min_x, max_x = min(all_x) - 1, max(all_x) + 1
    min_y, max_y = min(all_y) - 1, max(all_y) + 1
    
    ax.set_xlim(min_x, max_x)
    ax.set_ylim(min_y, max_y)
    ax.set_aspect('equal', adjustable='box')  # 保持网格坐标比例
    ax.set_title("Grid with edges colored by 'Total c'")
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    draw_grid_with_edges("result.txt")
