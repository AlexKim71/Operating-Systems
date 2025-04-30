# «Operating Systems»
### [National University "Odesa Polytechnic"](https://op.edu.ua/)
**📅 Timeline:** 1 course 2 semester  
**🎯 Major:** Computer Science | 122  

#### Relevant Coursework:
| Course | Semester | Credits | Key Topics | Projects |
|--------|----------|---------|------------|----------|
| **[Operating Systems](course-link)** | Fall 2023 | 4 | Process scheduling, Memory management | [Kernel Module](project-link) |
| Algorithms | Spring 2024 | 3 | Graph theory, Dynamic programming | [Pathfinder](project-link) |

#### 🖥️ Full Stack OS & Virtualization Technologies

| Категория            | Технологии                                                                 | Стикеры                                                                                                                                 |
|----------------------|----------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------|
| **Виртуализация**    | `QEMU` `KVM` `VirtualBox` `VMware` `Hyper-V`                              | ![QEMU](https://img.shields.io/badge/QEMU-FF6600?logo=qemu&logoColor=white) ![KVM](https://img.shields.io/badge/KVM-4695EB?logo=virtualbox) |
| **Контейнеризация**  | `Docker` `LXC` `Kubernetes` `Podman`                                      | ![Docker](https://img.shields.io/badge/Docker-2496ED?logo=docker&logoColor=white) ![K8s](https://img.shields.io/badge/Kubernetes-326CE5?logo=kubernetes) |
| **Облачные платформы** | `AWS EC2` `Google Cloud` `Azure VMs` `DigitalOcean`                    | ![AWS](https://img.shields.io/badge/AWS-FF9900?logo=amazon-aws) ![Azure](https://img.shields.io/badge/Azure-0089D6?logo=microsoft-azure) |
| **CI/CD**           | `GitHub Actions` `GitLab CI` `Jenkins` `ArgoCD`                          | ![GH Actions](https://img.shields.io/badge/GitHub_Actions-2088FF?logo=github-actions) ![Jenkins](https://img.shields.io/badge/Jenkins-D24939?logo=jenkins) |
| **Удалённые серверы** | `SSH` `Ansible` `Terraform` `Nginx` `Apache`                           | ![SSH](https://img.shields.io/badge/SSH-53A1D3?logo=ssh) ![Terraform](https://img.shields.io/badge/Terraform-7B42BC?logo=terraform) |
| **Виртуальная реальность** | `OpenXR` `Unity` `Unreal Engine` `WebXR`                            | ![OpenXR](https://img.shields.io/badge/OpenXR-FF3C00?logo=openxr) ![Unreal](https://img.shields.io/badge/Unreal-0E1128?logo=unreal-engine) |
| **Мониторинг**       | `Prometheus` `Grafana` `ELK Stack` `Zabbix`                              | ![Prometheus](https://img.shields.io/badge/Prometheus-E6522C?logo=prometheus) ![Grafana](https://img.shields.io/badge/Grafana-F46800?logo=grafana) |

### 🚀 Деплой-пример через GitHub Actions
```yaml
name: Deploy to AWS
on: [push]
jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: make build
      - uses: aws-actions/configure-aws-credentials@v3
        with:
          aws-access-key-id: ${{ secrets.AWS_ACCESS_KEY }}
          aws-secret-access-key: ${{ secrets.AWS_SECRET_KEY }}
      - run: aws s3 sync ./dist s3://my-bucket
