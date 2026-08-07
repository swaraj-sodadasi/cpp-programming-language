/*
 * =====================================================================================
 * CONCEPT        : Abstraction in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the principles, mechanisms, and architectural benefits of Abstraction:
 *
 *                  1. Interface vs. Implementation : Separating "what" an object does (public 
 *                                                    interface) from "how" it does it (hidden logic).
 *                  2. Abstract Base Classes (ABCs) : Pure virtual functions (`= 0`) defining strict 
 *                                                    type contracts that derived classes must fulfill.
 *                  3. Polymorphic Decoupling       : High-level client modules operating strictly on 
 *                                                    abstract pointers/references without depending 
 *                                                    on concrete types (Dependency Inversion).
 *                  4. Virtual Destructors          : Essential for safe polymorphic deletion and 
 *                                                    leak-free resource unwinding.
 *                  5. Implementation Complexity    : Hiding intricate details (network encryption, 
 *                                                    disk buffers, retry logic) behind clean APIs.
 *
 * RESOLVED WARNING: Used `requiredBytes` parameter in `LocalStorageEngine::verifyDiskSpace()`
 *                  diagnostic log, resolving GCC/Clang `-Wunused-parameter` warning.
 *
 * TIME COMPLEXITY  : Dynamic Interface Dispatch : O(1) constant time (via vtable indirection).
 * SPACE COMPLEXITY : Memory Footprint per Object : 8 bytes vptr + concrete data member sizes.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. ABSTRACT BASE CLASS (INTERFACE CONTRACT)
// Defines the high-level storage abstraction. Contains ZERO implementation details.
// =====================================================================================
class ICloudStorageEngine {
public:
    // Virtual Destructor: Crucial for safe polymorphic memory release
    virtual ~ICloudStorageEngine() noexcept = default;

    // Pure Virtual Functions (Interface Methods)
    virtual void uploadData(const string& filename, const string& payload) = 0;
    [[nodiscard]] virtual string downloadData(const string& filename) const = 0;
    virtual void deleteData(const string& filename) = 0;
    [[nodiscard]] virtual size_t getStorageUsageBytes() const noexcept = 0;
    [[nodiscard]] virtual string getProviderName() const noexcept = 0;
};

// =====================================================================================
// 2. CONCRETE IMPLEMENTATION A: AWS S3 STORAGE ENGINE
// Encapsulates complex AWS S3 network, chunking, and encryption logic behind the API.
// =====================================================================================
class AwsS3StorageEngine : public ICloudStorageEngine {
private:
    string bucketName_;
    string region_;
    size_t simulatedStorageUsageBytes_{0};
    bool isEncrypted_{true};

    // Hidden Private Helper Methods (Implementation Details)
    void authenticateAwsCredentials() const {
        cout << "      [AWS S3 INTERNAL] Validating IAM access keys and SigV4 signatures...\n";
    }

    void encryptPayload(const string& payload) const {
        cout << "      [AWS S3 INTERNAL] Encrypting " << payload.size() 
             << " bytes with KMS AES-256 GCM...\n";
    }

public:
    AwsS3StorageEngine(string bucketName, string region)
        : bucketName_(std::move(bucketName)), region_(std::move(region)) {
        cout << "    [AwsS3 Ctor] Connected to S3 Bucket: '" << bucketName_ 
             << "' in region: '" << region_ << "'\n";
    }

    ~AwsS3StorageEngine() noexcept override {
        cout << "    [AwsS3 Dtor] Disconnecting S3 session for bucket: '" << bucketName_ << "'\n";
    }

    void uploadData(const string& filename, const string& payload) override {
        authenticateAwsCredentials();
        if (isEncrypted_) {
            encryptPayload(payload);
        }
        simulatedStorageUsageBytes_ += payload.size();
        cout << "    [AWS S3 UPLOAD SUCCESS] Uploaded '" << filename << "' (" 
             << payload.size() << " bytes) to s3://" << bucketName_ << "/" << filename << "\n";
    }

    [[nodiscard]] string downloadData(const string& filename) const override {
        authenticateAwsCredentials();
        cout << "    [AWS S3 DOWNLOAD] Fetched payload stream for: s3://" << bucketName_ << "/" << filename << "\n";
        return "[Decrypted S3 Stream Payload for " + filename + "]";
    }

    void deleteData(const string& filename) override {
        authenticateAwsCredentials();
        cout << "    [AWS S3 DELETE] Removed object: s3://" << bucketName_ << "/" << filename << "\n";
    }

    [[nodiscard]] size_t getStorageUsageBytes() const noexcept override {
        return simulatedStorageUsageBytes_;
    }

    [[nodiscard]] string getProviderName() const noexcept override {
        return "Amazon Web Services S3 (" + region_ + ")";
    }
};

// =====================================================================================
// 3. CONCRETE IMPLEMENTATION B: LOCAL FILE SYSTEM STORAGE ENGINE
// Encapsulates local disk drive buffer management and file path abstractions.
// =====================================================================================
class LocalStorageEngine : public ICloudStorageEngine {
private:
    string mountPath_;
    size_t localStorageBytes_{0};

    // Hidden Private Helper Methods
    void verifyDiskSpace(size_t requiredBytes) const {
        cout << "      [LOCAL DISK INTERNAL] Checking POSIX statvfs for path: " << mountPath_ 
             << " (" << requiredBytes << " bytes required)...\n";
    }

public:
    explicit LocalStorageEngine(string mountPath)
        : mountPath_(std::move(mountPath)) {
        cout << "    [LocalStorage Ctor] Mounted local storage drive at: '" << mountPath_ << "'\n";
    }

    ~LocalStorageEngine() noexcept override {
        cout << "    [LocalStorage Dtor] Unmounting local storage at: '" << mountPath_ << "'\n";
    }

    void uploadData(const string& filename, const string& payload) override {
        verifyDiskSpace(payload.size());
        localStorageBytes_ += payload.size();
        cout << "    [LOCAL STORAGE UPLOAD] Saved file to disk: " << mountPath_ << "/" << filename 
             << " (" << payload.size() << " bytes)\n";
    }

    [[nodiscard]] string downloadData(const string& filename) const override {
        cout << "    [LOCAL STORAGE DOWNLOAD] Reading disk sector at: " << mountPath_ << "/" << filename << "\n";
        return "[Local File System Content for " + filename + "]";
    }

    void deleteData(const string& filename) override {
        cout << "    [LOCAL STORAGE DELETE] Unlinked file inode: " << mountPath_ << "/" << filename << "\n";
    }

    [[nodiscard]] size_t getStorageUsageBytes() const noexcept override {
        return localStorageBytes_;
    }

    [[nodiscard]] string getProviderName() const noexcept override {
        return "Local POSIX FileSystem (" + mountPath_ + ")";
    }
};

// =====================================================================================
// 4. HIGH-LEVEL CLIENT ORCHESTRATOR (DEPENDENCY INVERSION)
// Operates PURELY on the `ICloudStorageEngine` abstraction.
// Completely agnostic of whether AWS, Local Disk, or Azure is being used!
// =====================================================================================
class SystemBackupManager {
private:
    shared_ptr<ICloudStorageEngine> storageEngine_;

public:
    // Dependency Injection via Abstract Interface Pointer
    explicit SystemBackupManager(shared_ptr<ICloudStorageEngine> engine)
        : storageEngine_(std::move(engine)) {
        if (!storageEngine_) {
            throw invalid_argument("Storage engine abstraction pointer cannot be null!");
        }
    }

    void performSystemBackup(const string& backupName, const string& systemStateData) {
        cout << "\n  --- INITIATING SYSTEM BACKUP ON: " << storageEngine_->getProviderName() << " ---\n";
        storageEngine_->uploadData(backupName + ".bak", systemStateData);
        cout << "  Current Total Usage: " << storageEngine_->getStorageUsageBytes() << " bytes\n";
    }

    void restoreSystemBackup(const string& backupName) const {
        cout << "\n  --- INITIATING SYSTEM RESTORE FROM: " << storageEngine_->getProviderName() << " ---\n";
        string data = storageEngine_->downloadData(backupName + ".bak");
        cout << "  Restored Data Stream: " << data << "\n";
    }

    // Dynamic Provider Swap at Runtime
    void switchStorageProvider(shared_ptr<ICloudStorageEngine> newEngine) {
        if (!newEngine) {
            throw invalid_argument("New storage engine pointer cannot be null!");
        }
        cout << "\n  [PROVIDER SWAP] Switching storage backend from '" 
             << storageEngine_->getProviderName() << "' to '" 
             << newEngine->getProviderName() << "'\n";
        storageEngine_ = std::move(newEngine);
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Abstraction demonstration analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. POLYMORPHIC INSTANTIATION OF CONCRETE ENGINES
    // =====================================================================================
    cout << "\n================ 1. INSTANTIATING CONCRETE STORAGE ENGINES ================\n";

    // High-level shared pointers bound to abstract interface `ICloudStorageEngine`
    auto awsEngine = make_shared<AwsS3StorageEngine>("enterprise-backup-bucket-" + to_string(userInputValue), "us-east-1");
    auto localEngine = make_shared<LocalStorageEngine>("/var/backups/node_" + to_string(userInputValue));

    // =====================================================================================
    // 2. HIGH-LEVEL SYSTEM BACKUP MANAGER USING ABSTRACTION
    // =====================================================================================
    cout << "\n================ 2. EXECUTING OPERATIONS THROUGH ABSTRACT INTERFACE ================\n";

    // SystemBackupManager accepts the abstract interface pointer
    SystemBackupManager backupManager(awsEngine);

    // Backup executed on AWS S3 (High-level manager doesn't know/care about AWS internal SDK logic)
    backupManager.performSystemBackup("Snapshot_v1.0", "SYS_STATE_FLAGS=0x4F8A;LOGS_ENABLED=TRUE;");
    backupManager.restoreSystemBackup("Snapshot_v1.0");

    // =====================================================================================
    // 3. RUNTIME PROVIDER SWAPPING (DEPENDENCY INVERSION IN ACTION)
    // =====================================================================================
    cout << "\n================ 3. RUNTIME PROVIDER SWAPPING ================\n";

    // Swap backend to Local Storage at runtime without modifying SystemBackupManager code!
    backupManager.switchStorageProvider(localEngine);

    backupManager.performSystemBackup("Snapshot_v2.0", "SYS_STATE_FLAGS=0x9B1C;LOGS_ENABLED=TRUE;");
    backupManager.restoreSystemBackup("Snapshot_v2.0");

    // =====================================================================================
    // 4. DIRECT INTERFACE POLYMORPHISM VIA VECTOR
    // =====================================================================================
    cout << "\n================ 4. HETEROGENEOUS ABSTRACTION CONTAINER ================\n";

    vector<shared_ptr<ICloudStorageEngine>> storagePool;
    storagePool.push_back(awsEngine);
    storagePool.push_back(localEngine);

    cout << "  - Iterating polymorphically over storage pool interface:\n";
    for (const auto& engine : storagePool) {
        cout << "    * Provider: " << setw(40) << left << engine->getProviderName()
             << " | Total Usage: " << engine->getStorageUsageBytes() << " bytes\n";
    }

    // =====================================================================================
    // 5. INVARIANT ENFORCEMENT & ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 5. ABSTRACTION INVARIANT GUARDS ================\n";

    try {
        cout << "  - Attempting to initialize SystemBackupManager with nullptr interface...\n";
        SystemBackupManager faultyManager(nullptr);
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    cout << "\n  - Leaving main scope (Polymorphic virtual destructors execute cleanly):\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ ABSTRACTION IN OOP SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Abstraction Principle | Implementation Technique          | Primary Architectural Benefit     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Abstract Base Class   | Class with pure virtual `virtual f()=0;` | Enforces mandatory type interface|\n"
         << "| Implementation Hiding | Private methods & internal state  | Conceals complexity from callers  |\n"
         << "| Polymorphic Dispatch  | Virtual methods & `override`      | Dynamic runtime algorithm choice  |\n"
         << "| Dependency Inversion  | Depend on Interface (`ICloud`), not | Loose coupling; zero implementation|\n"
         << "|                       | concrete types (`AwsS3Engine`)    | lock-in for callers               |\n"
         << "| Virtual Destructor    | `virtual ~Interface() noexcept;`  | Guarantees clean teardown order   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}