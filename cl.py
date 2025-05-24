import os
import argparse
from collections import defaultdict

DEFAULT_EXTS = ['.py', '.java', '.c', '.cpp', '.h', '.js', '.html', '.css', '.md','.kent','.g4']

def main():
    parser = argparse.ArgumentParser(description='统计目录下文件的行数')
    parser.add_argument('--path', default='.', help='要统计的目录路径，默认为当前目录')
    parser.add_argument('--ext', nargs='*', default=None, help=f'文件扩展名过滤（如 py txt），默认：{DEFAULT_EXTS}')
    parser.add_argument('--exclude-dir', nargs='*', default=['.git', 'venv'], help='要排除的目录名')
    parser.add_argument('-v', '--verbose', action='store_true', help='显示每个文件的详细信息')
    args = parser.parse_args()

    # 处理扩展名参数
    allowed_exts = ['.' + e.lstrip('.').lower() for e in args.ext] if args.ext else [e.lower() for e in DEFAULT_EXTS]

    stats = defaultdict(lambda: {'files': 0, 'lines': 0})
    total_files = 0
    total_lines = 0
    file_details = []

    for root, dirs, files in os.walk(args.path):
        # 过滤排除目录
        dirs[:] = [d for d in dirs if d not in args.exclude_dir]
        
        for file in files:
            file_path = os.path.join(root, file)
            ext = os.path.splitext(file)[1].lower()

            if ext not in allowed_exts:
                continue

            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    line_count = sum(1 for _ in f)
            except UnicodeDecodeError:
                print(f"⚠️ 跳过非文本文件：{file_path}")
                continue
            except Exception as e:
                print(f"❌ 读取文件出错：{file_path} - {str(e)}")
                continue

            # 更新统计信息
            stats[ext]['files'] += 1
            stats[ext]['lines'] += line_count
            total_files += 1
            total_lines += line_count
            
            if args.verbose:
                file_details.append((file_path, line_count))

    # 输出结果
    if args.verbose:
        print("\n详细文件统计：")
        for path, count in file_details:
            print(f"  {path}: {count} 行")

    print("\n📊 按扩展名统计：")
    for ext in sorted(stats.keys()):
        print(f"  {ext}: {stats[ext]['files']} 个文件, {stats[ext]['lines']} 行")

    print(f"\n✅ 总计：{total_files} 个文件, {total_lines} 行")

if __name__ == '__main__':
    main()
