Software Architecture: Code must be split into functional modules with clear public interfaces (headers) and hidden implementations (cpp files) to ensure high cohesion and low coupling.   
PDF
+1
C++ Best Practices: Use classes for encapsulation, preventing direct access to internal states. Apply RAII (Resource Acquisition Is Initialization) using standard containers like std::vector and std::unique_ptr to manage memory automatically.   
PDF
+1
Project Structure: Organize code into include/ (public headers), src/ (implementations), and tests/. Avoid exposing internal implementation details to the client.   
PDF
+1
Build System: Use CMake to manage the build process. Key commands include project, find_package(OpenCV REQUIRED), add_executable, and target_link_libraries. CMake simplifies linking external dependencies like OpenCV and Eigen and supports out-of-source builds (build/ directory).   
PDF
+2
Quality Control: Compile with warnings enabled (-Wall -Wextra -Wpedantic). Use assertions (assert) to check invariants during development. Apply Address and Undefined Behavior Sanitizers (-fsanitize=address,undefined) for debugging memory issues.   
PDF
+2
Git Workflow: Maintain a clean version control history using frequent, logically grouped commits. Exclude build artifacts using a .gitignore file. Utilize branches (e.g., feature/lsh) for parallel development.